#ifndef STRUCTS_H
#define STRUCTS_H

enum oper_type {
    PLUS,
    UN_MINUS,
    BIN_MINUS,
    MULTIPLY,
    DIVIDE,
    MOD,
    OPEN_B,
    CLOSE_B,
    SIN,
    COS,
    TAN,
    CTG,
    POW,
    ABS,
    SQRT,
    LN,
    CONSTANT,
    UNDEF
};

enum error_type
{ CORRECT, INVALID_LEXEM, INVALID_BALANCE, UNFINISHED, EMPTY_STACK, INVALID_EXPRESSION, MEMORY };

struct stack_d {
    double data;
    struct stack_d* down;
};

// int get_priority(enum oper_type oper);
struct expr_item {
    enum oper_type type;
    double value;  // на случай если это константа
    int priority;  // приоритет операции
    int (*calculate)(double value, struct stack_d** top);  //указатель на функцию которая будет все вычислять
};

struct stack {
    struct expr_item data;
    struct stack* down;
};

struct stack_d* stack_d_init();
void stack_d_push(struct stack_d** top, double val);
double stack_d_top(struct stack_d* top);
void stack_d_pop(struct stack_d** top);
int stack_d_is_empty(struct stack_d* top);
void stack_d_destroy(struct stack_d* top);

struct stack* stack_init();
void stack_push(struct stack** top, struct expr_item val);
struct expr_item stack_top(struct stack* top);
void stack_pop(struct stack** top);
int stack_is_empty(struct stack* top);
void stack_destroy(struct stack* top);

struct vector {
    struct expr_item* data;
    int size;
    int capacity;
};

struct vector vector_init();
int vector_is_valid(struct vector vec);
void vector_realloc(struct vector* vec);
void vector_push_back(struct vector* vec, struct expr_item item);
struct expr_item vector_get_elem(struct vector* vec, int index);
void vector_destroy(struct vector* vec);

#endif