#include <ctype.h>
#include <stdio.h>
#include "calc.h"

typedef double (*BinaryOp)(double, double);

typedef struct Command
{
    char symbol;
    const char *name;
    BinaryOp func;
} Command;

#define MAX_COMMANDS 16
#define LEN 128

static Command commands[MAX_COMMANDS];
static int commandCount = 0;

static int register_command(char symbol, const char *name, BinaryOp func)
{
    if (commandCount >= MAX_COMMANDS) return -1;
    commands[commandCount].symbol = symbol;
    commands[commandCount].name = name;
    commands[commandCount].func = func;
    commandCount++;
    return 0;
}

static const Command *find_command(char symbol)
{
    for (int i = 0; i < commandCount; i++)
        if (commands[i].symbol == symbol) return &commands[i];
    return NULL;
}

static void print_menu()
{
    printf("Введите выражение, например: 2 + 3. Доступные операции:\n");
    for (int i = 0; i < commandCount; i++)
        printf("  %c - %s\n", commands[i].symbol, commands[i].name);
    printf("q - выход\n");
}

int main()
{
    register_command('+', "Сложение", add);
    register_command('-', "Вычитание", substract);
    register_command('*', "Умножение", multiply);
    register_command('/', "Деление", divide);

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

        double a, b;
        char op;
        if (parse_expression(p, &a, &op, &b) != 0)
        {
            printf("Ошибка: нужно число, знак операции и число, например 2 + 3\n");
            continue;
        }

        const Command *cmd = find_command(op);
        if (!cmd)
        {
            printf("Неизвестная операция '%c'\n", op);
            continue;
        }
        if (cmd->func == divide && b == 0)
        {
            printf("Ошибка: деление на ноль!\n");
            continue;
        }
        printf("Результат: %.5f\n", cmd->func(a, b));
    }
    return 0;
}