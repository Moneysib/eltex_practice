#include <stdio.h>
#include <string.h>
#include "perms.h"

#define LEN 256

static int input(const char *prompt, char *dst)
{
    printf("%s", prompt);
    if (!fgets(dst, LEN, stdin))
        return 0;
    size_t n = strcspn(dst, "\n");
    if (dst[n] == '\0')
    {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
    }
    dst[n] = '\0';
    return 1;
}

int main(void)
{
    unsigned mode = 0;
    int have = 0;
    char line[LEN];

    while (1)
    {
        printf("\n1 - ввести права\n2 - права файла по имени\n3 - изменить права (как chmod)\n0 - выход\n");
        if (!input("> ", line) || strcmp(line, "0") == 0)
            break;
        if (strcmp(line, "1") == 0)
        {
            input("Права (rwxr-xr-- или 754): ", line);
            if (parse_numeric(line, &mode) == 0 || parse_letters(line, &mode) == 0)
            {
                have = 1;
                print_mode(mode);
            }
            else
                printf("Ошибка: нужно 9 символов вида rwxr-xr-- или 3 цифры 0-7\n");
        }
        else if (strcmp(line, "2") == 0)
        {
            input("Имя файла: ", line);
            if (mode_from_file(line, &mode) == 0)
            {
                have = 1;
                print_mode(mode);
                printf("  (сравните с выводом: ls -l %s)\n", line);
            }
        }
        else if (strcmp(line, "3") == 0)
        {
            if (!have)
            {
                printf("Сначала задайте права (пункт 1 или 2)\n");
                continue;
            }
            input("Команда (g+w, a-wx, go=rw, 660): ", line);
            if (apply_chmod(&mode, line) == 0)
                print_mode(mode);
            else
                printf("Ошибка: неверная команда\n");
        }
        else
            printf("Неизвестная команда\n");
    }
    return 0;
}