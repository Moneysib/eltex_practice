#ifndef OPERATION_H_
#define OPERATION_H_

typedef int (*BinaryOp)(double a, double b, double *result);

typedef struct Operation
{
    char symbol;
    const char *name;
    BinaryOp func;
} Operation;

#define OPERATION_SYMBOL "operation"

#endif