#include "operation.h"

static int add(double a, double b, double *result)
{
    *result = a + b;
    return 0;
}

const Operation operation = {'+', "Сложение", add};