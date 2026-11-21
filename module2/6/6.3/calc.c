#include <ctype.h>
#include <stdlib.h>
#include "calc.h"

int parse_expression(const char *line, double *a, char *op, double *b)
{
    char *end;

    *a = strtod(line, &end);
    if (end == line) return -1;

    while (isspace((unsigned char)*end)) end++;
    if (*end == '\0') return -1;
    *op = *end++;

    const char *rest = end;
    *b = strtod(rest, &end);
    if (end == rest) return -1;

    while (isspace((unsigned char)*end)) end++;
    return *end == '\0' ? 0 : -1;
}