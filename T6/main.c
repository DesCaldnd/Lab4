#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include "../structs.h"

int eq(char l, char r)
{
    return l == r;
}

int hash_c(char c)
{
    return c;
}

PAIR(c, char, b, bool)
void del_p(struct pair_cb* p){}
UNORDERED_MAP(c, char, b, bool)
VECTOR(ei, struct node_t*)

struct map_cb;

enum oper_type {
    CONJUNCTION,
    DISJUNCTION,
    INVERSION,
    IMPLICATION,
    COIMPLICATION,
    XOR,
    EQUAL,
    SHEFFER,
    WEBB,
    OPEN_B,
    CLOSE_B,
    CONSTANT,
    VARIABLE,
    UNDEF
};

enum error_type
{ CORRECT, INVALID_LEXEM, INVALID_BALANCE, UNFINISHED, EMPTY_STACK, INVALID_EXPRESSION, MEMORY };

struct node_t;

struct expr_item {
    enum oper_type type;
    bool value;  // на случай если это константа
    char name;
    int priority;  // приоритет операции
    bool (*calculate)(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map);  //указатель на функцию которая будет все вычислять
};

struct node_t
{
    struct expr_item item;
    struct node_t* l, *r;
};


struct node_t* create_node(struct expr_item item, struct node_t* l, struct node_t* r);
void destroy_tree(struct node_t** node, void*);

struct stack {
    struct expr_item data;
    struct stack* down;
};

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

char* get_lexem(char* str, struct expr_item* answer, struct map_cb* map, int* has_left_operand);

struct vector expression_to_postfix(char* str, struct map_cb* map, enum error_type* error);
//если введенное выражение некорректно, вернет невалидный вектор
// Есть функция vector_is_valid, которая это и проверяет
// Также может быть невалидным если закончилась память, но разницы нет, мы работать не можем, выводи просто,
// что произошла ошибка

struct node_t* create_tree(struct vector expr);

bool calc_conjunction(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map);
bool calc_disjunction(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map);
bool calc_inversion(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map);
bool calc_implication(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map);
bool calc_coimplication(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map);
bool calc_xor(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map);
bool calc_equal(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map);
bool calc_sheffer(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map);
bool calc_webb(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map);
bool calc_constant(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map);
bool calc_variable(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map);

void work(FILE* in, char* path);
FILE* open_out(char* path);
int is_endl(int c);
void work_with_expr(struct String str, FILE* out);
long long power_ld_int(long long base, unsigned int pow);

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Incorrect argc\n");
        return 0;
    }

    srand(time(NULL));

    FILE *in = fopen(argv[1], "r");

    if (in == NULL)
    {
        printf("Unable to open input file");
        return 0;
    }

    work(in, argv[1]);

    fclose(in);
    return 0;
}

void work(FILE* in, char* path)
{
    FILE* out = NULL;
    while (out == NULL)
        out = open_out(path);

    struct String str = init_string_from_stream(in, is_endl);
    if (!is_valid_string(&str))
    {
        fclose(out);
        printf("Bad alloc");
        return;
    }
    work_with_expr(str, out);

    destroy_string(&str);
    fclose(out);
}

char from_int(int c)
{
    if (c < 10)
        return c + '0';
    else if (c < 36)
        return c - 10 + 'A';
    else
        return c - 36 + 'a';
}

FILE* open_out(char* path)
{
    size_t len = strlen(path);

    size_t start_ind = len - 1;

    while(path[start_ind] != '/' && path[start_ind] != '\\')
    {
        --start_ind;
    }
    ++start_ind;

    char *res = malloc(sizeof(char) * (start_ind + 10));
    if (res == NULL)
        return NULL;

    strncpy(res, path, start_ind);
    for (size_t i = 0; i < 5; ++i)
    {
        res[start_ind + i] = from_int(rand() % 62);
    }
    strcpy(res + start_ind + 5, ".txt");

    FILE* out = fopen(res, "w");
    free(res);
    return out;
}

int is_endl(int c)
{
    return c != '\n';
}

void work_with_expr(struct String str, FILE* out)
{
    struct map_cb map = init_map_cb(eq);
    if (!is_valid_map_cb(&map))
    {
        printf("Bad alloc\n");
        return;
    }

    enum error_type state;
    struct vector expr = expression_to_postfix(str.data, &map, &state);

    if (!vector_is_valid(expr) || !is_valid_map_cb(&map))
    {
        printf("Something went wrong\n");
        vector_destroy(&expr);
        destroy_map_cb(&map);
        return;
    }

    struct node_t *tree = create_tree(expr);
    vector_destroy(&expr);

    if (tree == NULL)
    {
        printf("Bad alloc\n");
        destroy_map_cb(&map);
        return;
    }

    struct node_ml_cb* it = map.data.begin->next;
    while (it != NULL)
    {
        fprintf(out, "%c ", it->data.pair.first);
        it = it->next;
    }

    fprintf(out, "Result\n");

    size_t max_pow = power_ld_int(2, map.size);

    for (size_t i = 0; i < max_pow; ++i)
    {
        struct node_ml_cb* nit = map.data.begin->next;
        size_t counter = 0;
        while (nit != NULL)
        {
            nit->data.pair.second = i & (((size_t)1) << counter);
            ++counter;
            fprintf(out, "%d ", nit->data.pair.second);
            nit = nit->next;
        }

        bool res = tree->item.calculate(tree->item.value, tree->item.name, tree->l, tree->r, &map);

        fprintf(out, "%d\n", res);
    }

    destroy_tree(&tree, NULL);
    destroy_map_cb(&map);
}

long long power_ld_int(long long base, unsigned int pow)
{
    long long result = 1, last_multiplier = base;

    for (int i = 0; i < sizeof(unsigned int) * 8; ++i)
    {
        if (pow & 1u << i)
        {

            result *= last_multiplier;
        }
        last_multiplier *= last_multiplier;
    }
    return result;
}
void push_all_by_priority(struct stack** top, struct vector* expression, int cur_priority) {
    while (!stack_is_empty(*top) && vector_is_valid(*expression) &&
           stack_top(*top).priority >= cur_priority && (stack_top(*top).priority != 3 || cur_priority != 3)) {
        vector_push_back(expression, stack_top(*top));
        stack_pop(top);
    }
}

int push_all(struct stack** top, struct vector* expression) {
    while (!stack_is_empty(*top) && vector_is_valid(*expression)) {
        if (stack_top(*top).type == OPEN_B)
        {
            vector_destroy(expression);
            return 0;
        }
        vector_push_back(expression, stack_top(*top));
        stack_pop(top);
    }
    return 1;
}

struct node_t* create_tree(struct vector expr)
{
    struct vector_ei stack = init_vector_ei(expr.size);
    if (!is_valid_vector_ei(&stack))
    {
        return NULL;
    }

    for (size_t i = 0; i < expr.size; ++i)
    {
        struct expr_item it = expr.data[i];
        if (it.type == CONSTANT || it.type == VARIABLE)
        {
            struct node_t* node = create_node(it, NULL, NULL);
            if (node == NULL)
            {
                for_all_vector_ei(&stack, destroy_tree, NULL);
                destroy_vector_ei(&stack);
                return NULL;
            }
            push_vector_ei(&stack, node, destroy_tree);
        } else if (it.type == INVERSION)
        {
            struct node_t* l = top_vector_ei(&stack);
            pop_vector_ei(&stack);
            struct node_t* node = create_node(it, l, NULL);
            if (node == NULL)
            {
                destroy_tree(&l, NULL);
                for_all_vector_ei(&stack, destroy_tree, NULL);
                destroy_vector_ei(&stack);
                return NULL;
            }
            push_vector_ei(&stack, node, destroy_tree);
        } else
        {
            struct node_t* r = top_vector_ei(&stack);
            pop_vector_ei(&stack);
            struct node_t* l = top_vector_ei(&stack);
            pop_vector_ei(&stack);
            struct node_t* node = create_node(it, l, r);
            if (node == NULL)
            {
                destroy_tree(&l, NULL);
                destroy_tree(&r, NULL);
                for_all_vector_ei(&stack, destroy_tree, NULL);
                destroy_vector_ei(&stack);
                return NULL;
            }
            push_vector_ei(&stack, node, destroy_tree);
        }
    }
    struct node_t* answer = top_vector_ei(&stack);
    destroy_vector_ei(&stack);
    return answer;
}

struct vector expression_to_postfix(char* str, struct map_cb* map, enum error_type* error) {
    struct vector res = vector_init();
    if (!vector_is_valid(res))
    {
        *error = MEMORY;
        return res;
    }

    struct stack* oper_stack = stack_init();
    if (oper_stack == NULL) {
        vector_destroy(&res);
        *error = MEMORY;
        return res;
    }

    int has_left_operand = 0;

    while (*str != '\0') {
        struct expr_item item;
        str = get_lexem(str, &item, map, &has_left_operand);
        if (!is_valid_map_cb(map))
        {
            vector_destroy(&res);
            stack_destroy(oper_stack);
            *error = MEMORY;
            return res;
        }
        if (item.type == UNDEF) {
            vector_destroy(&res);
            stack_destroy(oper_stack);
            *error = INVALID_LEXEM;
            return res;
        }
        if (item.type == CONSTANT || item.type == VARIABLE) {
            vector_push_back(&res, item);
            if (!vector_is_valid(res)) {
                stack_destroy(oper_stack);
                *error = MEMORY;
                return res;
            }
        } else if (item.type == CLOSE_B) {
            push_all_by_priority(&oper_stack, &res, item.priority);
            if (!stack_is_empty(oper_stack) && vector_is_valid(res) && stack_top(oper_stack).type == OPEN_B) {
                stack_pop(&oper_stack);
            } else {
                if (vector_is_valid(res))
                {
                    *error = INVALID_BALANCE;
                } else
                {
                    *error = MEMORY;
                }
                if (vector_is_valid(res)) vector_destroy(&res);
                stack_destroy(oper_stack);
                return res;
            }
        } else if (item.type == OPEN_B) {
            stack_push(&oper_stack, item);
        } else {
            push_all_by_priority(&oper_stack, &res, item.priority);
            stack_push(&oper_stack, item);
            if (!vector_is_valid(res)) {
                stack_destroy(oper_stack);
                *error = MEMORY;
                return res;
            }
        }
    }

    *error = CORRECT;
    int status = push_all(&oper_stack, &res);
    stack_destroy(oper_stack);

    if (!vector_is_valid(res))
    {
        *error = MEMORY;
    } else if (status == 0)
    {
        *error = INVALID_BALANCE;
    } else if(has_left_operand == 0)
    {
        vector_destroy(&res);
        *error = UNFINISHED;
    } else if(res.size == 0)
    {
        vector_destroy(&res);
        *error = INVALID_EXPRESSION;
    }
    return res;
}

char* get_lexem(char* str, struct expr_item* answer, struct map_cb* map, int* has_left_operand) {
    answer->value = 0;
    answer->name = 0;
    if (strncmp("&", str, 1) == 0 && *has_left_operand)
    {
        answer->type = CONJUNCTION;
        answer->priority = 2;
        answer->calculate = calc_conjunction;
        *has_left_operand = 0;
        str += 1;
    } else if (strncmp("|", str, 1) == 0 && *has_left_operand)
    {
        answer->type = DISJUNCTION;
        answer->priority = 1;
        answer->calculate = calc_disjunction;
        *has_left_operand = 0;
        str += 1;
    } else if (strncmp("->", str, 2) == 0 && *has_left_operand)
    {
        answer->type = IMPLICATION;
        answer->priority = 1;
        answer->calculate = calc_implication;
        *has_left_operand = 0;
        str += 2;
    } else if (strncmp("+>", str, 2) == 0 && *has_left_operand)
    {
        answer->type = COIMPLICATION;
        answer->priority = 2;
        answer->calculate = calc_coimplication;
        *has_left_operand = 0;
        str += 2;
    } else if (strncmp("<>", str, 2) == 0 && *has_left_operand)
    {
        answer->type = XOR;
        answer->priority = 1;
        answer->calculate = calc_xor;
        *has_left_operand = 0;
        str += 2;
    } else if (strncmp("=", str, 1) == 0 && *has_left_operand)
    {
        answer->type = EQUAL;
        answer->priority = 1;
        answer->calculate = calc_equal;
        *has_left_operand = 0;
        str += 1;
    } else if (strncmp("!", str, 1) == 0 && *has_left_operand)
    {
        answer->type = SHEFFER;
        answer->priority = 2;
        answer->calculate = calc_sheffer;
        *has_left_operand = 0;
        str += 1;
    } else if (strncmp("?", str, 1) == 0 && *has_left_operand)
    {
        answer->type = WEBB;
        answer->priority = 2;
        answer->calculate = calc_webb;
        *has_left_operand = 0;
        str += 1;
    } else if (strncmp("~", str, 1) == 0 && !*has_left_operand)
    {
        answer->type = INVERSION;
        answer->priority = 3;
        answer->calculate = calc_inversion;
        *has_left_operand = 0;
        str += 1;
    } else if (strncmp("(", str, 1) == 0 && !*has_left_operand) {
        answer->type = OPEN_B;
        answer->priority = -1;
        *has_left_operand = 0;
        str += 1;
    } else if (strncmp(")", str, 1) == 0 && *has_left_operand) {
        answer->type = CLOSE_B;
        answer->priority = 0;
        *has_left_operand = 1;
        str += 1;
    } else if ((*str == '1' || *str == 0) && !*has_left_operand)
    {
        answer->type = CONSTANT;
        answer->priority = 0;
        answer->calculate = calc_constant;
        answer->value = *str == '1' ? true : false;
        *has_left_operand = 1;
        str += 1;
    } else if (isalpha(*str))
    {
        answer->type = VARIABLE;
        answer->priority = 0;
        answer->calculate = calc_variable;
        answer->name = *str;
        *has_left_operand = 1;
        insert_map_cb(map, *str, false, del_p);
        str += 1;
    }
    return str;
}

bool calc_conjunction(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map)
{
    bool lhs = l->item.calculate(l->item.value, l->item.name, l->l, l->r, map);
    bool rhs = r->item.calculate(r->item.value, r->item.name, r->l, r->r, map);

    return lhs && rhs;
}

bool calc_disjunction(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map)
{
    bool lhs = l->item.calculate(l->item.value, l->item.name, l->l, l->r, map);
    bool rhs = r->item.calculate(r->item.value, r->item.name, r->l, r->r, map);

    return lhs || rhs;
}

bool calc_inversion(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map)
{
    bool lhs = l->item.calculate(l->item.value, l->item.name, l->l, l->r, map);

    return !lhs;
}

bool calc_implication(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map)
{
    bool lhs = l->item.calculate(l->item.value, l->item.name, l->l, l->r, map);
    bool rhs = r->item.calculate(r->item.value, r->item.name, r->l, r->r, map);

    return lhs <= rhs;
}

bool calc_coimplication(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map)
{
    bool lhs = l->item.calculate(l->item.value, l->item.name, l->l, l->r, map);
    bool rhs = r->item.calculate(r->item.value, r->item.name, r->l, r->r, map);

    return lhs > rhs;
}

bool calc_xor(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map)
{
    bool lhs = l->item.calculate(l->item.value, l->item.name, l->l, l->r, map);
    bool rhs = r->item.calculate(r->item.value, r->item.name, r->l, r->r, map);

    return lhs != rhs;
}

bool calc_equal(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map)
{
    bool lhs = l->item.calculate(l->item.value, l->item.name, l->l, l->r, map);
    bool rhs = r->item.calculate(r->item.value, r->item.name, r->l, r->r, map);

    return lhs == rhs;
}

bool calc_sheffer(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map)
{
    bool lhs = l->item.calculate(l->item.value, l->item.name, l->l, l->r, map);
    bool rhs = r->item.calculate(r->item.value, r->item.name, r->l, r->r, map);

    return !(lhs && rhs);
}

bool calc_webb(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map)
{
    bool lhs = l->item.calculate(l->item.value, l->item.name, l->l, l->r, map);
    bool rhs = r->item.calculate(r->item.value, r->item.name, r->l, r->r, map);

    return !(lhs || rhs);
}

bool calc_constant(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map)
{
    return value;
}

bool calc_variable(bool value, char name, struct node_t* l, struct node_t* r, struct map_cb* map)
{
    struct pair_cb* pair = find_map_cb(map, name);
    if (pair != NULL)
        return pair->second;
    else
        return false;
}
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
//--------------------------------------


struct node_t* create_node(struct expr_item item, struct node_t* l, struct node_t* r)
{
    struct node_t* res = malloc(sizeof(struct node_t));
    if (res == NULL)
        return res;

    res->item = item;
    res->l = l;
    res->r = r;
}

void destroy_tree(struct node_t** node, void*)
{
    if ((*node)->l != NULL)
        destroy_tree(&(*node)->l, NULL);
    if ((*node)->r != NULL)
        destroy_tree(&(*node)->r, NULL);

    free(*node);
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