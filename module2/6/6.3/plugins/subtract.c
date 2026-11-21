#include "operation.h"

static int subtract(double a, double b, double *result)
{
    *result = a - b;
    return 0;
}

const Operation operation = {'-', "Вычитание", subtract};