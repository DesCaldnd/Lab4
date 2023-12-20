//
// Created by Des Caldnd on 12/19/2023.
//
#include "../structs.h"
#include <string.h>
#include <ctype.h>

int hash_s(struct String str);

PAIR(s, struct String, s, struct String)

UNORDERED_MAP(s, struct String, s, struct String)

void del_pair(struct pair_ss* pair);
int eq_l(struct String lhs, struct String rhs);
int is_word(int c);
int is_endl(int c);
int work(FILE* in, FILE* out);

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("Incorrect argc\n");
        return 0;
    }

	if (strcmp(argv[1], argv[2]) == 0)
	{
		printf("Files must be different\n");
		return 0;
	}

    FILE* in = fopen(argv[1], "r+");

    if (in == NULL)
    {
        printf("Error while opening input file\n");
        return 0;
    }

    FILE* out = fopen(argv[2], "w");

    if (out == NULL)
    {
        fclose(in);
        printf("Error while opening output file\n");
        return 0;
    }

    int status = work(in, out);

    if (status)
        printf("SUCCESS\n");
    else
        printf("FAIL\n");

    fclose(in);
    fclose(out);

    return 0;
}

int work_str(struct String str, FILE* out, struct map_ss* defines)
{
	struct pair_ss* data = find_map_ss(defines, str);
	if (data == NULL)
		fprintf(out, "%s", str.data);
	else
		fprintf(out, "%s", data->second.data);
}

int work(FILE* in, FILE* out)
{
    struct map_ss defines = init_map_ss(eq_l);
    if (!is_valid_map_ss(&defines))
        return 0;

    struct String str = init_string_from_stream(in, is_word);
    if (!is_valid_string(&str))
    {
        destroy_map_ss(&defines);
        return 0;
    }

    while (strcmp(str.data, "#define") == 0 && !feof(in))
    {
        struct String name = init_string_from_stream(in, is_word);
        struct String value = init_string_from_stream(in, is_endl);

        if (!is_valid_string(&name) || !is_valid_string(&value))
        {
            destroy_string(&str);
            destroy_string(&name);
            destroy_string(&value);
            struct node_ml_ss* it = defines.data.begin->next;

            while (it != NULL)
            {
                del_pair(&it->data.pair);
                it = it->next;
            }
            destroy_map_ss(&defines);
            return 0;
        }

        int status = insert_map_ss(&defines, name, value, del_pair);

        if (!status)
        {
            destroy_string(&str);
            destroy_string(&name);
            destroy_string(&value);
            struct node_ml_ss* it = defines.data.begin->next;

            while (it != NULL)
            {
                del_pair(&it->data.pair);
                it = it->next;
            }
            destroy_map_ss(&defines);
            return 0;
        }

        destroy_string(&str);

        str = init_string_from_stream(in, is_word);
        if (!is_valid_string(&str))
        {
            destroy_string(&str);
            struct node_ml_ss* it = defines.data.begin->next;

            while (it != NULL)
            {
                del_pair(&it->data.pair);
                it = it->next;
            }
            destroy_map_ss(&defines);
            return 0;
        }
    }

	work_str(str, out, &defines);
    fseek(in, -1, SEEK_CUR);

    char c = getc(in);

    while (!feof(in))
    {
        if (!is_word(c))
        {
            putc(c, out);
        } else
        {
            fseek(in, -1, SEEK_CUR);
            destroy_string(&str);
            str = init_string_from_stream(in, is_word);

            if (!is_valid_string(&str))
            {
                destroy_string(&str);
                struct node_ml_ss* it = defines.data.begin->next;

                while (it != NULL)
                {
                    del_pair(&it->data.pair);
                    it = it->next;
                }
                destroy_map_ss(&defines);
                return 0;
            }

			work_str(str, out, &defines);

            if (!feof(in))
                fseek(in, -1, SEEK_CUR);
        }

        c = getc(in);
    }

    destroy_string(&str);
    struct node_ml_ss* it = defines.data.begin->next;

    while (it != NULL)
    {
        del_pair(&it->data.pair);
        it = it->next;
    }
    destroy_map_ss(&defines);

    return 1;
}

int char_to_i(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'A' && c <= 'Z')
        return c - 'A' + 10;
    else
        return c - 'a' + 36;
}

int hash_s(struct String str)
{
    int res = 0;
    size_t ind = 0;
    while (str.data[ind] != '\0')
    {
        res = char_to_i(str.data[ind]) + res * 62;
        ++ind;
    }
    return res;
}

void del_pair(struct pair_ss* pair)
{
    destroy_string(&pair->first);
    destroy_string(&pair->second);
}

int eq_l(struct String lhs, struct String rhs)
{
    return strcmp(lhs.data, rhs.data) == 0;
}

int is_word(int c)
{
    return !isspace(c);
}

int is_endl(int c)
{
    return c != '\n' && c != EOF;
}