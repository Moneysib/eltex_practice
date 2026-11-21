#include "operation.h"

static int multiply(double a, double b, double *result)
{
    *result = a * b;
    return 0;
}

const Operation operation = {'*', "Умножение", multiply};