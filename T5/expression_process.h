#ifndef EXPRESSION_PROCESS_H
#define EXPRESSION_PROCESS_H
#include "structs.h"


char* get_lexem(char* str, struct expr_item* answer, int* has_left_operand);

struct vector expression_to_postfix(char* str, enum error_type* error);
//если введенное выражение некорректно, вернет невалидный вектор
// Есть функция vector_is_valid, которая это и проверяет
// Также может быть невалидным если закончилась память, но разницы нет, мы работать не можем, выводи просто,
// что произошла ошибка

double calculate_expression(struct vector expression, enum error_type* error);

int calc_sin(double value, struct stack_d** top);
int calc_cos(double value, struct stack_d** top);
int calc_tan(double value, struct stack_d** top);
int calc_ctg(double value, struct stack_d** top);
int calc_sqrt(double value, struct stack_d** top);
int calc_ln(double value, struct stack_d** top);
int calc_abs(double value, struct stack_d** top);
int calc_pow(double value, struct stack_d** top);
int calc_plus(double value, struct stack_d** top);
int calc_mod(double value, struct stack_d** top);
int calc_un_minus(double value, struct stack_d** top);
int calc_bin_minus(double value, struct stack_d** top);
int calc_multiply(double value, struct stack_d** top);
int calc_divide(double value, struct stack_d** top);
int calc_const(double value, struct stack_d** top);

#endif