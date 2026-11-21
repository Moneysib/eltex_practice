#include "operation.h"

static int divide(double a, double b, double *result)
{
    if (b == 0) return -1;
    *result = a / b;
    return 0;
}

const Operation operation = {'/', "Деление", divide};