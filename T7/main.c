#include "../structs.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

enum instr_type
{ ASSIGN, PRINT, PRINT_ALL };

enum oper_type
{ PLUS, MINUS, MULTIPLY, DIVIDE, MOD, NO, ERR };

struct MemoryCell
{
    struct String key;
    int value;
};

VECTOR(mem, struct MemoryCell)

int cmpf(const void* lhs, const void* rhs);
void del_mem(struct MemoryCell* mem, void* d);
int work(FILE* in);
int is_needed_sym(int c);
enum instr_type get_instr_type(struct String str);
void assign(struct vector_mem* vars, struct String str);
void print(struct vector_mem* vars, struct String str);
void print_all(struct vector_mem* vars);
struct MemoryCell* find(struct vector_mem* vars, struct String name);
enum oper_type parse_expr(struct vector_mem* vars, struct String str, size_t start, int* l, int* r);

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Incorrect argc\n");
        return 0;
    }

    FILE* in = fopen(argv[1], "r");

    if (in == NULL)
    {
        printf("Unable to open input file\n");
        return 0;
    }

    int status = work(in);

    if (status)
        printf("SUCCESS\n");
    else
        printf("FAIL\n");

    fclose(in);
    return 0;
}

int is_needed_sym(int c)
{
    return (!isspace(c) && c != ';') || c == ' ';
}

int cmpf(const void* lhs, const void* rhs)
{
    return strcmp(((struct String*)lhs)->data, ((struct String*)rhs)->data);
}

int work(FILE* in)
{
    struct vector_mem vars = init_vector_mem(10);

    if (!is_valid_vector_mem(&vars))
        return 0;

    struct String str;

    while(!feof(in))
    {
        str = init_string_from_stream(in, is_needed_sym);

        if (!is_valid_string(&str))
        {
            for_all_vector_mem(&vars, del_mem, NULL);
            destroy_vector_mem(&vars);
            return 0;
        }
        enum instr_type type = get_instr_type(str);

        switch (type)
        {
            case ASSIGN:
                assign(&vars, str);
                break;
            case PRINT:
                print(&vars, str);
                break;
            case PRINT_ALL:
                print_all(&vars);
                break;
        }
        destroy_string(&str);

        if (!is_valid_vector_mem(&vars))
        {
            return 0;
        }
    }

    for_all_vector_mem(&vars, del_mem, NULL);
    destroy_vector_mem(&vars);
    return 1;
}

void del_mem(struct MemoryCell* mem, void* d)
{
    destroy_string(&mem->key);
}

enum instr_type get_instr_type(struct String str)
{
    if (strncmp(str.data, "print ", 6) == 0)
        return PRINT;
    else if (strcmp(str.data, "print") == 0)
        return PRINT_ALL;
    else
        return ASSIGN;
}

void assign(struct vector_mem* vars, struct String str)
{
    size_t end_l_name = 0;
    while (str.data[end_l_name] != '=')
    {
        ++end_l_name;
    }
    struct String l_name = init_string_size(end_l_name + 1);
    if (!is_valid_string(&l_name))
        return;
    for (size_t i = 0; i < end_l_name; ++i)
    {
        push_string_c(&l_name, str.data[i]);
    }
    int f_val, s_val;
    enum oper_type oper = parse_expr(vars, str, end_l_name + 1, &f_val, &s_val);

    int result = 0;

    switch (oper)
    {
        case PLUS:
            result = f_val + s_val;
            break;
        case MINUS:
            result = f_val - s_val;
            break;
        case MULTIPLY:
            result = f_val * s_val;
            break;
        case DIVIDE:
            if (s_val != 0)
                result = f_val / s_val;
            break;
        case MOD:
            result = f_val % s_val;
            break;
        case NO:
            result = f_val;
            break;
        case ERR:
            destroy_string(&l_name);
            destroy_vector_mem(vars);
            break;
    }

    struct MemoryCell* mem = find(vars, l_name);

    if (mem == NULL)
    {
        struct MemoryCell new_mem;
        new_mem.key = l_name;
        new_mem.value = result;
        push_vector_mem(vars, new_mem, del_mem);
        if (!is_valid_vector_mem(vars))
        {
            destroy_string(&l_name);
            return;
        }
        qsort(vars->data, vars->size, sizeof(struct MemoryCell), cmpf);
    } else
    {
        destroy_string(&l_name);
        mem->value = result;
    }
}

void print(struct vector_mem* vars, struct String str)
{
    struct String name = init_string_size(str.size - 5);

    for (size_t i = 0; i < str.size - 6; ++i)
    {
        push_string_c(&name, str.data[i + 6]);
    }
    struct MemoryCell* mem = find(vars, name);
    destroy_string(&name);
    if (mem != NULL)
    {
        printf("Name: %s, value: %d\n\n", mem->key.data, mem->value);
    }
}

void print_all(struct vector_mem* vars)
{
    for (size_t i = 0; i < vars->size; ++i)
    {
        printf("Name: %s, value: %d\n", vars->data[i].key.data, vars->data[i].value);
    }
    printf("\n");
}

struct MemoryCell* find(struct vector_mem* vars, struct String name)
{
    if (vars->size == 0)
        return NULL;
    struct MemoryCell mem;
    mem.key = name;
    mem.value = 0;
    long long l = 0, r = vars->size - 1, mid = (l + r) / 2;

    while(l <= r)
    {
        int state = cmpf(&mem, &vars->data[mid]);

        if (state == 0)
            return &vars->data[mid];
        if (state < 0)
            r = mid - 1;
        else
            l = mid + 1;
        mid = (l + r) / 2;
    }

    return NULL;
}

enum oper_type parse_expr(struct vector_mem* vars, struct String str, size_t start, int* l, int* r)
{
    if (isalpha(str.data[start]))
    {
        size_t tmp = start + 1;
        while (isalpha(str.data[tmp]))
        { ++tmp; }
        size_t size = tmp - start;
        struct String tmp_str = init_string_size(size + 1);
        if (!is_valid_string(&tmp_str))
            return ERR;

        for (size_t i = 0; i < size; ++i)
        {
            push_string_c(&tmp_str, str.data[i + start]);
        }

        struct MemoryCell* mem = find(vars, tmp_str);
        destroy_string(&tmp_str);

        if (mem == NULL)
            return ERR;

        *l = mem->value;
        start = tmp;
    } else
    {
        char* end;
        *l = strtol(str.data + start, &end, 10);
        start = end - str.data;
    }

    enum oper_type type;

    switch (str.data[start])
    {
        case '+':
            type = PLUS;
            break;
        case '-':
            type = MINUS;
            break;
        case '*':
            type = MULTIPLY;
            break;
        case '/':
            type = DIVIDE;
            break;
        case '%':
            type = MOD;
            break;
        case '\0':
            type = NO;
            break;
    }
    ++start;
    *r = 0;
    if (type != NO)
    {
        if (isalpha(str.data[start]))
        {
            size_t tmp = start + 1;
            while (isalpha(str.data[tmp]))
            { ++tmp; }
            size_t size = tmp - start;
            struct String tmp_str = init_string_size(size + 1);
            if (!is_valid_string(&tmp_str))
                return ERR;

            for (size_t i = 0; i < size; ++i)
            {
                push_string_c(&tmp_str, str.data[i + start]);
            }

            struct MemoryCell* mem = find(vars, tmp_str);
            destroy_string(&tmp_str);

            if (mem == NULL)
                return ERR;

            *r = mem->value;
            start = tmp;
        } else
        {
            char* end;
            *r = strtol(str.data + start, &end, 10);
            start = end - str.data;
        }
    }
    return type;
}