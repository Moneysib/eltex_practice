#include <ctype.h>
#include <stdio.h>
#include "calc.h"

#define LEN 128

int main()
{
    char line[LEN];
    printf("Введите выражение, например: 2 + 3 (операции: + - * /).\nq - выход\n");

    while(1)
    {
        printf("\n> ");
        if (!fgets(line, sizeof line, stdin))
            break;

        const char *p = line;
        while (isspace((unsigned char)*p))
            p++;
        if (*p == '\0') continue;
        if (*p == 'q') break;

        double a, b, result;
        char op;
        if (parse_expression(p, &a, &op, &b) != 0)
        {
            printf("Ошибка: нужно число, знак операции и число, например 2 + 3\n");
            continue;
        }

        switch(op)
        {
            case '+':
                result = add(a, b); break;
            case '-':
                result = substract(a, b); break;
            case '*':
                result = multiply(a, b); break;
            case '/':
                if (b == 0)
                {
                    printf("Деление на ноль!\n");
                    continue;
                }
                result = divide(a, b);
                break;
            default:
                printf("Неизвестная операция '%c'\n", op);
                continue;
        }
        printf("Результат: %.5f\n", result);
    }
    return 0;
}