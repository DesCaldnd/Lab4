//
// Created by Des Caldnd on 12/19/2023.
//
#include "../structs.h"
#include <string.h>
#include <stdio.h>
#include <regex.h>
#include <time.h>
#include <ctype.h>
#include <math.h>

VECTOR(i, int)

PAIR(c, char, vi, struct vector_i)

int hash_c(char c);
int eq(char lhs, char rhs);
void del_pair(struct pair_cvi* pair);
int work(FILE* in);
int is_not_endstr(int c);
int always_false(int i){ return 0; }
void del_i(int* i, void* p){}


UNORDERED_MAP(c, char, vi, struct vector_i)


int load(struct String str, regex_t* reg, struct map_cvi* map);
int save(struct String str, regex_t* reg, struct map_cvi* map);
int rand_(struct String str, regex_t* reg, struct map_cvi* map);
int concat(struct String str, regex_t* reg, struct map_cvi* map);
int free_(struct String str, regex_t* reg, struct map_cvi* map);
int remove_(struct String str, regex_t* reg, struct map_cvi* map);
int copy(struct String str, regex_t* reg, struct map_cvi* map);
int sort_i(struct String str, regex_t* reg, struct map_cvi* map);
int sort_d(struct String str, regex_t* reg, struct map_cvi* map);
int shuffle(struct String str, regex_t* reg, struct map_cvi* map);
int stats(struct String str, regex_t* reg, struct map_cvi* map);
int print_1(struct String str, regex_t* reg, struct map_cvi* map);
int print_2(struct String str, regex_t* reg, struct map_cvi* map);
int print_all(struct String str, regex_t* reg, struct map_cvi* map);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Incorrect argc\n");
        return 0;
    }

    FILE* in = fopen(argv[1], "r+");

    if (in == NULL)
    {
        printf("Error while opening input file");
        return 0;
    }

    srand(time(NULL));
    int status = work(in);

    if (!status)
        printf("Memory error");

    fclose(in);
    return 0;
}

int work(FILE* in)
{
    regex_t regexes[14];
    int status = 0;
    status += regcomp(&regexes[0], "Load [[:upper:]], (../)*([[:alnum:]]+/)*[[:alnum:]]+.txt", REG_EXTENDED);
    status += regcomp(&regexes[1], "Save [[:upper:]], (../)*([[:alnum:]]+/)*[[:alnum:]]+.txt", REG_EXTENDED);
    status += regcomp(&regexes[2], "Rand [[:upper:]], [1-9][0-9]*, -?[1-9][0-9]*, -?[1-9][0-9]*", REG_EXTENDED);
    status += regcomp(&regexes[3], "Concat [[:upper:]], [[:upper:]]", 0);
    status += regcomp(&regexes[4], "Free\\([[:upper:]]\\)", REG_EXTENDED);
    status += regcomp(&regexes[5], "Remove [[:upper:]], [1-9][0-9]*, [1-9][0-9]*", 0);
    status += regcomp(&regexes[6], "Copy [[:upper:]], [1-9][0-9]*, [1-9][0-9]*, [[:upper:]]", 0);
    status += regcomp(&regexes[7], "Sort [[:upper:]]-", 0);
    status += regcomp(&regexes[8], "Sort [[:upper:]]\\+", REG_EXTENDED);
    status += regcomp(&regexes[9], "Shuffle [[:upper:]]", 0);
    status += regcomp(&regexes[10], "Stats [[:upper:]]", 0);
    status += regcomp(&regexes[11], "Print [[:upper:]], [1-9][0-9]*, [1-9][0-9]*", 0);
    status += regcomp(&regexes[12], "Print [[:upper:]], [1-9][0-9]*", 0);
    status += regcomp(&regexes[13], "Print [[:upper:]], all", 0);
    if (status != 0)
    {
        for (int i = 0; i < 14; ++i)
        {
            regfree(&regexes[i]);
        }
        return 0;
    }

    int (*functions[14])(struct String, regex_t*, struct map_cvi*);

    functions[0] = load;
    functions[1] = save;
    functions[2] = rand_;
    functions[3] = concat;
    functions[4] = free_;
    functions[5] = remove_;
    functions[6] = copy;
    functions[7] = sort_d;
    functions[8] = sort_i;
    functions[9] = shuffle;
    functions[10] = stats;
    functions[11] = print_2;
    functions[12] = print_1;
    functions[13] = print_all;

    struct map_cvi arrays = init_map_cvi(eq);
    if (!is_valid_map_cvi(&arrays))
    {
        for (int i = 0; i < 14; ++i)
        {
            regfree(&regexes[i]);
        }
        return 0;
    }

    while (!feof(in))
    {
        char buf;
        struct String str = init_string_from_stream_buf(in, &buf, is_not_endstr, always_false);

        if (!is_valid_string(&str))
        {
            struct node_ml_cvi *it = arrays.data.begin->next;

            while (it != NULL)
            {
                del_pair(&it->data.pair);
                it = it->next;
            }

            destroy_map_cvi(&arrays);
            for (int i = 0; i < 14; ++i)
            {
                regfree(&regexes[i]);
            }
            return 0;
        }

        if (!is_empty_string(&str))
        {
            int found = 0;
            if (buf == ';')
            {
                for (int i = 0; i < 14; ++i)
                {
                    int res = functions[i](str, &regexes[i], &arrays);
                    if (res == 1)
                    {
                        found = 1;
                        break;
                    } else if (res == -1)
                    {
                        destroy_string(&str);
                        struct node_ml_cvi *it = arrays.data.begin->next;

                        while (it != NULL)
                        {
                            del_pair(&it->data.pair);
                            it = it->next;
                        }

                        destroy_map_cvi(&arrays);
                        for (int i = 0; i < 14; ++i)
                        {
                            regfree(&regexes[i]);
                        }
                        return 0;
                    }
                }
            }
            if (!found)
            {
                printf("Following expression is incorrect:\n\t%s%c\n", str.data, buf);
            }

        }

        destroy_string(&str);
        if (!is_valid_map_cvi(&arrays))
        {
            for (int i = 0; i < 14; ++i)
            {
                regfree(&regexes[i]);
            }
            return 0;
        }
        char c = getc(in);
        while (!feof(in) && isspace(c))
        {
            c = getc(in);
        }
        if (!feof(in))
        {
            fseek(in, -1, SEEK_CUR);
        }
    }

    struct node_ml_cvi *it = arrays.data.begin->next;

    while (it != NULL)
    {
        del_pair(&it->data.pair);
        it = it->next;
    }

    destroy_map_cvi(&arrays);
    for (int i = 0; i < 14; ++i)
    {
        regfree(&regexes[i]);
    }
    return 1;
}

int hash_c(char c)
{
    return c;
}

//int hash_s(struct String str)
//{
//    int hash = 0;
//    for (size_t i = 0; str.data[i] != '\0'; ++i)
//    {
//        hash ^= ((int)str.data[i]) << i % ((sizeof(str) - sizeof(char)) * 8);
//    }
//    return hash;
//}

int eq(char lhs, char rhs)
{
    return lhs == rhs;
}

void del_pair(struct pair_cvi* pair)
{
    destroy_vector_i(&pair->second);
}

int is_not_endstr(int c)
{
    return c != ';' && c != '\n' && c != EOF;
}

int load(struct String str, regex_t* reg, struct map_cvi* map)
{
    int res = regexec(reg, str.data, 0, NULL, 0);

    if (res == REG_NOMATCH)
        return 0;

    char* filepath = malloc(sizeof(char) * (str.size - 8));
    if (filepath == NULL)
        return -1;
    char name;

//    name = str.data[5];
//    strcpy(filepath, str.data + 8);
    sscanf(str.data, "Load %c, %s", &name, filepath);

    FILE* in = fopen(filepath, "r");

    if (in == NULL)
    {
        free(filepath);
        return -1;
    }

    struct vector_i vec = init_vector_i(10);
    if (!is_valid_vector_i(&vec))
    {
        fclose(in);
        free(filepath);
        return -1;
    }

    while (!feof(in))
    {
        int data, counter;
        counter = fscanf(in, "%d", &data);
        if (counter == 1)
        {
            push_vector_i(&vec, data, del_i);
            if (!is_valid_vector_i(&vec))
            {
                free(filepath);
                fclose(in);
                return -1;
            }
        } else
        {
            char tmp = getc(in);
            while(!feof(in) && !isspace(tmp))
            {
                tmp = getc(in);
            }
        }
    }

    struct pair_cvi* pair = find_map_cvi(map, name);

    int status = 1;

    if (pair != NULL)
    {
        destroy_vector_i(&pair->second);
        pair->second = vec;
    } else
    {
        status = insert_map_cvi(map, name, vec, del_pair);
        if (!status)
            destroy_vector_i(&vec);
    }


    free(filepath);
    fclose(in);
    return status ? 1 : -1;
}

int save(struct String str, regex_t* reg, struct map_cvi* map)
{
    int res = regexec(reg, str.data, 0, NULL, 0);

    if (res == REG_NOMATCH)
        return 0;

    char* filepath = malloc(sizeof(char) * (str.size - 8));
    if (filepath == NULL)
        return -1;
    char name;

    sscanf(str.data, "Save %c, %s", &name, filepath);

    struct pair_cvi* pair = find_map_cvi(map, name);

    if (pair == NULL)
    {
        printf("Save instruction interruption caused by absence of such array\n");
        free(filepath);
        return 1;
    }

    FILE* out = fopen(filepath, "w");

    if (out == NULL)
    {
        free(filepath);
        return -1;
    }

    for (size_t i = 0; i < pair->second.size; ++i)
    {
        fprintf(out, "%d ", pair->second.data[i]);
    }

    free(filepath);
    fclose(out);
    return 1;
}

int rand_in_range(int l, int r)
{
    return (rand() % (r - l + 1)) + l;
}

int rand_(struct String str, regex_t* reg, struct map_cvi* map)
{
    int res = regexec(reg, str.data, 0, NULL, 0);

    if (res == REG_NOMATCH)
        return 0;

    char name;
    size_t count;
    int l, r;
    sscanf(str.data, "Rand %c, %zu, %d, %d", &name, &count, &l, &r);

    struct vector_i vec = init_vector_i(count);
    if (!is_valid_vector_i(&vec))
    {
        return -1;
    }

    if (l > r)
    {
        int tmp = l;
        l = r;
        r = tmp;
    }

    for (size_t i = 0; i < count; ++i)
    {
        push_vector_i(&vec, rand_in_range(l, r), del_i);
    }

    struct pair_cvi* pair = find_map_cvi(map, name);

    int status = 1;

    if (pair != NULL)
    {
        destroy_vector_i(&pair->second);
        pair->second = vec;
    } else
    {
        status = insert_map_cvi(map, name, vec, del_pair);
        if (!status)
            destroy_vector_i(&vec);
    }

    return status ? 1 : -1;
}

int concat(struct String str, regex_t* reg, struct map_cvi* map)
{
    int res = regexec(reg, str.data, 0, NULL, 0);

    if (res == REG_NOMATCH)
        return 0;

    char name_l, name_r;
    sscanf(str.data, "Concat %c, %c", &name_l, &name_r);

    struct pair_cvi *l, *r;
    l = find_map_cvi(map, name_l);
    r = find_map_cvi(map, name_r);

    if (l == NULL || r == NULL)
    {
        printf("One of arrays does not exist\n");
        return 1;
    }

    realloc_vector_i(&l->second, l->second.size + r->second.size, del_i);

    if (!is_valid_vector_i(&l->second))
        return -1;

    for (size_t i = 0; i < r->second.size; ++i)
    {
        push_vector_i(&l->second, r->second.data[i], del_i);
    }

    return 1;
}

int free_(struct String str, regex_t* reg, struct map_cvi* map)
{
    int res = regexec(reg, str.data, 0, NULL, 0);

    if (res == REG_NOMATCH)
        return 0;

    char name;
    sscanf(str.data, "Free(%c)", &name);

    struct pair_cvi* pair = find_map_cvi(map, name);

    if (pair != NULL)
        pair->second.size = 0;
    else
        printf("No such array: %c\n", name);

    return 1;
}

int remove_(struct String str, regex_t* reg, struct map_cvi* map)
{
    int res = regexec(reg, str.data, 0, NULL, 0);

    if (res == REG_NOMATCH)
        return 0;

    char name;
    size_t start, count;
    sscanf(str.data, "Remove %c, %zu, %zu", &name, &start, &count);

    struct pair_cvi* pair = find_map_cvi(map, name);

    if (pair != NULL)
    {
        size_t size = pair->second.size;

        if (size < start)
        {
            printf("No such index in array: %zu", start);
        } else
        {
            if (size < start + count)
            {
                count = size - start;
                printf("Not enough elements, count will be reduced to: %zu", count);
            }
            for (size_t i = start + count; i < size; ++i)
            {
                pair->second.data[i - count] = pair->second.data[i];
            }
            pair->second.size -= count;
        }
    } else
    {
        printf("No such array: %c\n", name);
    }
    return 1;
}

int copy(struct String str, regex_t* reg, struct map_cvi* map)
{
    int res = regexec(reg, str.data, 0, NULL, 0);

    if (res == REG_NOMATCH)
        return 0;

    char name_l, name_r;
    size_t start, end;
    sscanf(str.data, "Copy %c, %zu, %zu, %c", &name_l, &start, &end, &name_r);

    if (end < start)
    {
        printf("Incorrect range\n");
        return 1;
    }

    struct pair_cvi* l, *r;
    l = find_map_cvi(map, name_l);
    if (l == NULL)
    {
        printf("No such array: %c", name_l);
        return 1;
    }

    r = find_map_cvi(map, name_r);

    struct vector_i vec = init_vector_i(end - start + 1);

    if (!is_valid_vector_i(&vec))
        return -1;

    for (size_t i = start; i <= end; ++i)
    {
        push_vector_i(&vec, l->second.data[i], del_i);
    }

    int status = 1;
    if (r != NULL)
    {
        destroy_vector_i(&r->second);
        r->second = vec;
    } else
    {
        status = insert_map_cvi(map, name_r, vec, del_pair);
        if (!status)
            destroy_vector_i(&vec);
    }
    return status ? 1 : -1;
}

int increase(const void* lhs, const void* rhs)
{
    int l = *((int*)lhs), r = *((int*)rhs);
    return l - r;
}

int decrease(const void* lhs, const void* rhs)
{
    int l = *((int*)lhs), r = *((int*)rhs);
    return r - l;
}

int rand_order(const void* lhs, const void* rhs)
{
//    return rand_in_range(-1, 1);
    return rand() % 2 == 0 ? -1 : 1;
}


int sort_i(struct String str, regex_t* reg, struct map_cvi* map)
{
    int res = regexec(reg, str.data, 0, NULL, 0);

    if (res == REG_NOMATCH)
        return 0;

    char name;
    sscanf(str.data, "Sort %c", &name);

    struct pair_cvi* pair = find_map_cvi(map, name);

    if (pair == NULL)
    {
        printf("No such array: %c", name);
    } else
    {
        qsort(pair->second.data, pair->second.size, sizeof(int), increase);
    }

    return 1;
}

int sort_d(struct String str, regex_t* reg, struct map_cvi* map)
{
    int res = regexec(reg, str.data, 0, NULL, 0);

    if (res == REG_NOMATCH)
        return 0;

    char name;
    sscanf(str.data, "Sort %c", &name);

    struct pair_cvi* pair = find_map_cvi(map, name);

    if (pair == NULL)
    {
        printf("No such array: %c", name);
    } else
    {
        qsort(pair->second.data, pair->second.size, sizeof(int), decrease);
    }

    return 1;
}

int shuffle(struct String str, regex_t* reg, struct map_cvi* map)
{
    int res = regexec(reg, str.data, 0, NULL, 0);

    if (res == REG_NOMATCH)
        return 0;

    char name;
    sscanf(str.data, "Shuffle %c", &name);

    struct pair_cvi* pair = find_map_cvi(map, name);

    if (pair == NULL)
    {
        printf("No such array: %c", name);
    } else
    {
        qsort(pair->second.data, pair->second.size, sizeof(int), rand_order);
    }

    return 1;
}

int stats(struct String str, regex_t* reg, struct map_cvi* map)
{
    int res = regexec(reg, str.data, 0, NULL, 0);

    if (res == REG_NOMATCH)
        return 0;

    char name;
    sscanf(str.data, "Stats %c", &name);

    struct pair_cvi* pair = find_map_cvi(map, name);

    if (pair == NULL)
    {
        printf("No such array: %c", name);
    } else
    {
        if (pair->second.size == 0)
        {
            printf("Array is empty\n");
            return 1;
        }
        struct vector_i vec = init_vector_i(pair->second.size);
        if (!is_valid_vector_i(&vec))
            return -1;

        for (size_t i = 0; i < pair->second.size; ++i)
        {
            push_vector_i(&vec, pair->second.data[i], del_i);
        }
        qsort(vec.data, vec.size, sizeof(int), increase);

        int max_val = vec.data[0], cur_val = max_val, sum = max_val;
        size_t max_count = 1, cur_count = 1;

        for (size_t i = 1; i < vec.size; ++i)
        {
            sum += vec.data[i];
            if (vec.data[i] != cur_val)
            {
                cur_val = vec.data[i];
                cur_count = 1;
            } else
            {
                ++cur_count;
                if (cur_count >= max_count)
                {
                    max_count = cur_count;
                    max_val = cur_val;
                }
            }
        }
        destroy_vector_i(&vec);

        double average = ((double) sum) / (double) pair->second.size;
        int max_el = pair->second.data[0], min_el = max_el, far = max_el;
        size_t max_ind = 0, min_ind = 0;

        for (size_t i = 1; i < pair->second.size; ++i)
        {
            int elem = pair->second.data[i];
            if (fabs(average - far) < fabs(average - elem))
            {
                far = elem;
            }
            if (max_el < elem)
            {
                max_el = elem;
                max_ind = i;
            } else if (min_el > elem)
            {
                min_el = elem;
                min_ind = i;
            }
        }

        printf("Size: %zu\nMax element: %d, index: %zu\nMin element: %d, index: %zu\nMost frequent: %d\nAverage: %lf\nFarest: %d\n", pair->second.size, max_el, max_ind, min_el, min_ind, max_val, average, far);
    }
    return 1;
}

int print_1(struct String str, regex_t* reg, struct map_cvi* map)
{
    int res = regexec(reg, str.data, 0, NULL, 0);

    if (res == REG_NOMATCH)
        return 0;

    char name;
    size_t ind;
    sscanf(str.data, "Print %c, %zu", &name, &ind);

    struct pair_cvi* pair = find_map_cvi(map, name);

    if (pair == NULL)
    {
        printf("No such array: %c", name);
    } else
    {
        if (ind >= pair->second.size)
        {
            printf("Out of bound\n");
        } else
        {
            printf("%d\n", pair->second.data[ind]);
        }
    }

    return 1;
}

int print_2(struct String str, regex_t* reg, struct map_cvi* map)
{
    int res = regexec(reg, str.data, 0, NULL, 0);

    if (res == REG_NOMATCH)
        return 0;

    char name;
    size_t start, end;
    sscanf(str.data, "Print %c, %zu, %zu", &name, &start, &end);

    if (start > end)
    {
        printf("Incorrect range\n");
        return 1;
    }

    struct pair_cvi* pair = find_map_cvi(map, name);

    if (pair == NULL)
    {
        printf("No such array: %c", name);
    } else
    {
        size_t size = pair->second.size;
        if (end >= size)
        {
            printf("Out of bound\n");
        } else
        {
            for (size_t i = start; i <= end; ++i)
            {
                printf("%d ", pair->second.data[i]);
            }
            printf("\n");
        }
    }

    return 1;
}

int print_all(struct String str, regex_t* reg, struct map_cvi* map)
{
    int res = regexec(reg, str.data, 0, NULL, 0);

    if (res == REG_NOMATCH)
        return 0;

    char name;
    sscanf(str.data, "Print %c", &name);

    struct pair_cvi* pair = find_map_cvi(map, name);

    if (pair == NULL)
    {
        printf("No such array: %c", name);
    } else
    {
        for(size_t i = 0; i < pair->second.size; ++i)
        {
            printf("%d ", pair->second.data[i]);
        }
        printf("\n");
    }

    return 1;
}