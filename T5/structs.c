#include "structs.h"

#include <stdlib.h>

struct stack* stack_init() {
    struct stack* res = malloc(sizeof(struct stack));
    if (res != NULL) res->down = NULL;
    return res;
}

void stack_push(struct stack** top, struct expr_item val) {
    struct stack* tmp = malloc(sizeof(struct stack));
    if (tmp != NULL) {
        tmp->down = *top;
        tmp->data = val;
        *top = tmp;
    }
}

struct expr_item stack_top(struct stack* top) {
    return top->data;
}

void stack_pop(struct stack** top) {
    struct stack* tmp = *top;
    *top = (*top)->down;
    free(tmp);
}

int stack_is_empty(struct stack* top) {
    if (top != NULL)
        return top->down == NULL;
    else
        return 0;
}

void stack_destroy(struct stack* top) {
    if (top != NULL) {
        struct stack* tmp = top->down;
        free(top);
        stack_destroy(tmp);
    }
}

//---------------------------

struct stack_d* stack_d_init() {
    struct stack_d* res = malloc(sizeof(struct stack_d));
    if (res != NULL) res->down = NULL;
    return res;
}

void stack_d_push(struct stack_d** top, double val) {
    struct stack_d* tmp = malloc(sizeof(struct stack_d));
    if (tmp != NULL) {
        tmp->down = *top;
        tmp->data = val;
        *top = tmp;
    }
}

double stack_d_top(struct stack_d* top) { return top->data; }

void stack_d_pop(struct stack_d** top) {
    struct stack_d* tmp = *top;
    *top = (*top)->down;
    free(tmp);
}

int stack_d_is_empty(struct stack_d* top) {
    if (top != NULL)
        return top->down == NULL;
    else
        return 0;
}

void stack_d_destroy(struct stack_d* top) {
    if (top != NULL) {
        struct stack_d* tmp = top->down;
        free(top);
        stack_d_destroy(tmp);
    }
}

//-------------------

struct vector vector_init() {
    struct vector res;
    res.data = malloc(10 * sizeof(struct expr_item));
    res.size = 0;
    res.capacity = 10;
    return res;
}

int vector_is_valid(struct vector vec) { return vec.data != NULL; }

void vector_realloc(struct vector* vec) {
    vec->data = realloc(vec->data, vec->capacity * 2 * sizeof(struct expr_item));
    vec->capacity *= 2;
}

void vector_push_back(struct vector* vec, struct expr_item item) {
    if (vec->size >= vec->capacity) vector_realloc(vec);

    if (vector_is_valid(*vec)) {
        vec->data[vec->size] = item;
        ++vec->size;
    }
}

struct expr_item vector_get_elem(struct vector* vec, int index) {
    return vec->data[index];
}

void vector_destroy(struct vector* vec) {
    if (vec->data != NULL) free(vec->data);
    vec->data = NULL;
}

//---------------------------------

/*
int get_priority(enum oper_type oper)
{
    if (oper == PLUS || oper == BIN_MINUS)
        return 1;
    else if (oper == MULTIPLY || oper == DIVIDE)
        return 2;
    else if (oper == OPEN_B)
        return -1;
    else if (oper == CLOSE_B)
        return 0;
    else
        return 3;
}
*/