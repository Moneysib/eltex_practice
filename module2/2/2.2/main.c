#include <stdio.h>
#include "calc.h"

int main()
{
    char choice;
    double a, b;

    while(1)
    {
        printf("\n1 - Сложение\n2 - Вычитание\n3 - Умножение\n4 - Деление\n0 - Выход\n> ");
        if (scanf("%hhd", &choice) != 1) break;
        if (choice == 0) break;
        printf("Введите 2 числа через пробел: ");
        if (scanf("%lf %lf", &a, &b) != 2) break;

        switch(choice)
        {
            case 1:
                printf("Результат: %.5f\n", add(a,b));
                break;
            case 2:
                printf("Результат: %.5f\n", substract(a,b));
                break;
            case 3:
                printf("Результат: %.5f\n", multiply(a,b));
                break;
            case 4:
                if (b == 0) printf("Деление на ноль!");
                else printf("Результат: %.5f\n", divide(a,b));
                break;
            default:
                printf("Неизвестный номер команды!\n");
        }
    }
    return 0;
}