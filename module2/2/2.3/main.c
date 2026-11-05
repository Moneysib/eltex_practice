#include <stdio.h>
#include "calc.h"

typedef double (*BinaryOp)(double, double);

typedef struct Command
{
    const char *name;
    BinaryOp func;
} Command;

#define MAX_COMMANDS 16

static Command commands[MAX_COMMANDS];
static int commandCount = 0;

static int register_command(const char *name, BinaryOp func)
{
    if (commandCount >= MAX_COMMANDS) return -1;
    commands[commandCount].name = name;
    commands[commandCount].func = func;
    commandCount++;
    return 0;
}

static void print_menu()
{
    printf("\n");
    for (int i = 0; i < commandCount; i++)
        printf("%d - %s\n", i + 1, commands[i].name);
    printf("0 - Выход\n\n> ");
}

int main()
{
    register_command("Сложение", add);
    register_command("Вычитание", substract);
    register_command("Умножение", multiply);
    register_command("Деление", divide);

    char choice;
    double a, b;

    while(1)
    {
        print_menu();
        if (scanf("%hhd", &choice) != 1) break;
        if (choice == 0) break;
        if (choice < 1 || choice > commandCount)
        {
            printf("Неизвестный номер команды!\n");
            continue;
        }

        printf("Введите 2 числа через пробел: ");
        if (scanf("%lf %lf", &a, &b) != 2) break;

        BinaryOp op = commands[choice - 1].func;
        if (op == divide && b == 0)
        {
            printf("Ошибка: деление на ноль!\n");
            continue;
        }
        printf("Результат: %.5f\n", op(a,b));
    }
    return 0;
}