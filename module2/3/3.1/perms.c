#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "perms.h"

static const char PATTERN[] = "rwxrwxrwx";

int parse_numeric(const char *s, unsigned *mode)
{
    if (strlen(s) != 3)
        return -1;
    unsigned m = 0;
    for (int i = 0; i < 3; i++)
    {
        if (s[i] < '0' || s[i] > '7')
            return -1;
        m = (m << 3) | (unsigned)(s[i] - '0');
    }
    *mode = m;
    return 0;
}

int parse_letters(const char *s, unsigned *mode)
{
    if (strlen(s) != 9)
        return -1;
    unsigned m = 0;
    for (int i = 0; i < 9; i++)
    {
        if (s[i] == PATTERN[i])
            m |= 1u << (8 - i);
        else if (s[i] != '-')
            return -1;
    }
    *mode = m;
    return 0;
}

int apply_chmod(unsigned *mode, const char *cmd)
{
    if (parse_numeric(cmd, mode) == 0)
        return 0;

    unsigned who = 0;
    for (; *cmd == 'u' || *cmd == 'g' || *cmd == 'o' || *cmd == 'a'; cmd++)
        who |= *cmd == 'u' ? 0700 : *cmd == 'g' ? 0070 : *cmd == 'o' ? 0007 : 0777;
    if (who == 0)
        who = 0777;

    char op = *cmd;
    if (op != '+' && op != '-' && op != '=')
        return -1;
    cmd++;

    unsigned triple = 0;
    for (; *cmd; cmd++)
    {
        if
            (*cmd == 'r') triple |= 4;
        else if
            (*cmd == 'w') triple |= 2;
        else if
            (*cmd == 'x') triple |= 1;
        else
            return -1;
    }

    unsigned bits = ((triple << 6) | (triple << 3) | triple) & who;

    if (op == '+')
        *mode |= bits;
    else if (op == '-')
        *mode &= ~bits;
    else
        *mode = (*mode & ~who) | bits;
    return 0;
}

int mode_from_file(const char *path, unsigned *mode)
{
    struct stat st;
    if (stat(path, &st) != 0)
    {
        perror("stat");
        return -1;
    }
    *mode = (unsigned)st.st_mode & 0777;
    return 0;
}

void print_mode(unsigned mode)
{
    printf("  Буквенное: ");
    for (int i = 0; i < 9; i++)
        putchar(mode & (1u << (8 - i)) ? PATTERN[i] : '-');

    printf("\n  Цифровое:  %03o\n  Битовое:   ", mode);
    for (int i = 8; i >= 0; i--)
    {
        putchar(mode & (1u << i) ? '1' : '0');
        if (i % 3 == 0 && i != 0) putchar(' ');
    }
    printf("\n");
}