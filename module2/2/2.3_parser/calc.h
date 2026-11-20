#ifndef CALC_H_
#define CALC_H_

double add(double a, double b);
double substract(double a, double b);
double multiply(double a, double b);
double divide(double a, double b);

int parse_expression(const char *line, double *a, char *op, double *b);

#endif