#include "expression_process.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "../structs.h"

void main_hub(char* path);
void parse_file(FILE* in, char* path);
int is_needed(int c);
int no_skip(int c);
char* get_err_path(char *path);
void parse_expression(struct String str, FILE** out, char* err_path, size_t ind);


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Incorrect argc");
        return 0;
    }

    for (int i = 1; i < argc; ++i)
    {
        main_hub(argv[i]);
    }

    return 0;
}

void main_hub(char* path)
{
    FILE* in = fopen(path, "r");
    if (in == NULL)
    {
        printf("Unable to open file: %s\n", path);
        return;
    }

    parse_file(in, path);

    fclose(in);
}

void parse_file(FILE* in, char* path)
{
    FILE* out = NULL;
    char* err_path = get_err_path(path);
    if (err_path == NULL)
        return;

    char buf;
    size_t ind = 0;
    struct String str;

    while (!feof(in))
    {
        str = init_string_from_stream_buf(in, &buf, is_needed, no_skip);
        if (!is_valid_string(&str))
        {
            free(err_path);
            if (out != NULL)
                fclose(out);
            return;
        }

        if (!is_empty_string(&str))
        {
            parse_expression(str, &out, err_path, ind);
        }
        if (buf == '\n')
            ++ind;

        destroy_string(&str);
    }

    if (out != NULL)
        fclose(out);
    free(err_path);
}

int is_needed(int c)
{
    return !isspace(c);
}

int no_skip(int c)
{
    return c == '\n';
}

char* get_err_path(char *path)
{
    size_t len = strlen(path);
    char *res = malloc(sizeof(char) * (len + 5));
    if (res == NULL)
        return NULL;

    size_t start_ind = len - 1;

    while(path[start_ind] != '.')
    {
        --start_ind;
    }

    strncpy(res, path, start_ind);
    strcpy(res + start_ind, "_err");
    strcpy(res + start_ind + 4, path + start_ind);

    return res;
}

void print_expression(struct vector expr)
{
    for (size_t i = 0; i < expr.size; ++i)
    {
        switch (expr.data[i].type)
        {
            case PLUS:
                printf("+");
                break;
            case UN_MINUS:
                printf("-");
                break;
            case BIN_MINUS:
                printf("-");
                break;
            case MULTIPLY:
                printf("*");
                break;
            case DIVIDE:
                printf("/");
                break;
            case MOD:
                printf("%%");
                break;
            case SIN:
                printf("sin");
                break;
            case COS:
                printf("cos");
                break;
            case TAN:
                printf("tan");
                break;
            case CTG:
                printf("ctg");
                break;
            case POW:
                printf("^");
                break;
            case ABS:
                printf("abs");
                break;
            case SQRT:
                printf("sqrt");
                break;
            case LN:
                printf("ln");
                break;
            case CONSTANT:
                printf("%.2lf", expr.data[i].value);
                break;
        }
        printf(" ");
    }
}

void parse_expression(struct String str, FILE **out, char *err_path, size_t ind)
{
    enum error_type state;
    struct vector expr = expression_to_postfix(str.data, &state);

    if (state != CORRECT)
    {
        if (*out == NULL)
        {
            *out = fopen(err_path, "w");
            if (*out == NULL)
            {
                printf("Unable to open error file: %s\n", err_path);
                return;
            }
        }
        fprintf(*out, "Error in line %zu. ", ind);
        switch (state)
        {
            case INVALID_LEXEM:
                fprintf(*out, "Invalid lexem or operand/operation order violation");
                break;
            case INVALID_BALANCE:
                fprintf(*out, "Invalid bracket balance");
                break;
            case UNFINISHED:
                fprintf(*out, "Expression is unfinished");
                break;
            case INVALID_EXPRESSION:
                fprintf(*out, "Expression is empty");
                break;
            case MEMORY:
                fprintf(*out, "Error in memory allocation");
                break;
        }
        fprintf(*out, "\n\t%s\n", str.data);
        return;
    }

    double res = calculate_expression(expr, &state);

    if (state != CORRECT)
    {
        vector_destroy(&expr);
        if (*out == NULL)
        {
            *out = fopen(err_path, "w");
            if (*out == NULL)
            {
                printf("Unable to open error file: %s\n", err_path);
                return;
            }
        }
        fprintf(*out, "Error in line %zu. ", ind);
        switch (state)
        {
            case INVALID_EXPRESSION:
                fprintf(*out, "Expression is invalid (can return NAN)");
                break;
            case EMPTY_STACK:
                fprintf(*out, "Stack unexpectedly became empty");
                break;
            case MEMORY:
                fprintf(*out, "Error in memory allocation");
                break;
        }
        fprintf(*out, "\n\t%s\n", str.data);
        return;
    }

    printf("Expression: %s\nPostfix: ", str.data);
    print_expression(expr);
    printf("\nResult: %.2lf\n", res);

    vector_destroy(&expr);
}