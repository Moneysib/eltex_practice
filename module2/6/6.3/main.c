#define _DEFAULT_SOURCE
#include <ctype.h>
#include <dirent.h>
#include <dlfcn.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "calc.h"
#include "operation.h"

#define MAX_COMMANDS 16
#define LEN 128

static const Operation *commands[MAX_COMMANDS];
static void *handles[MAX_COMMANDS];
static int commandCount = 0;

static void load_commands(const char *dir)
{
    DIR *d = opendir(dir);
    if (!d)
    {
        perror(dir);
        return;
    }

    struct dirent *e;
    while ((e = readdir(d)) != NULL && commandCount < MAX_COMMANDS)
    {
        const char *ext = strrchr(e->d_name, '.');
        if (!ext || strcmp(ext, ".so") != 0) continue;

        char path[PATH_MAX];
        snprintf(path, sizeof path, "%s/%s", dir, e->d_name);

        void *h = dlopen(path, RTLD_LAZY);
        const Operation *op = h ? dlsym(h, OPERATION_SYMBOL) : NULL;
        if (!op)
        {
            fprintf(stderr, "%s: %s\n", path, dlerror());
            if (h) dlclose(h);
            continue;
        }
        handles[commandCount] = h;
        commands[commandCount++] = op;
    }
    closedir(d);
}

static const Operation *find_command(char symbol)
{
    for (int i = 0; i < commandCount; i++)
        if (commands[i]->symbol == symbol) return commands[i];
    return NULL;
}

static void print_menu()
{
    printf("Введите выражение, например: 2 + 3. Доступные операции:\n");
    for (int i = 0; i < commandCount; i++)
        printf("  %c - %s\n", commands[i]->symbol, commands[i]->name);
    printf("q - выход\n");
}

int main(int argc, char **argv)
{
    load_commands(argc > 1 ? argv[1] : "plugins");
    if (commandCount == 0)
    {
        fprintf(stderr, "Операции не найдены\n");
        return 1;
    }

    char line[LEN];
    print_menu();

    while(1)
    {
        printf("\n> ");
        if (!fgets(line, sizeof line, stdin)) break;

        const char *p = line;
        while (isspace((unsigned char)*p)) p++;
        if (*p == '\0') continue;
        if (*p == 'q') break;

        double a, b, result;
        char op;
        if (parse_expression(p, &a, &op, &b) != 0)
        {
            printf("Ошибка: нужно число, знак операции и число, например 2 + 3\n");
            continue;
        }

        const Operation *cmd = find_command(op);
        if (!cmd)
        {
            printf("Неизвестная операция '%c'\n", op);
            continue;
        }
        if (cmd->func(a, b, &result) != 0)
        {
            printf("Ошибка: операция не выполнена (деление на ноль?)\n");
            continue;
        }
        printf("Результат: %.5f\n", result);
    }

    for (int i = 0; i < commandCount; i++)
        dlclose(handles[i]);
    return 0;
}