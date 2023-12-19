//
// Created by Des Caldnd on 12/19/2023.
//
#include "../structs.h"

int hash_int(int k)
{ return k; }

PAIR(int, int, char, char)

UNORDERED_MAP(int, int, char, char)

void del_pair(struct pair_intchar*){}
int eq_l(int a, int b){ return a == b; }

int main()
{
    struct map_intchar map = init_map_intchar(eq_l);

    insert_map_intchar(&map, 200, 't', del_pair);
    insert_map_intchar(&map, 328, 'f', del_pair);
    insert_map_intchar(&map, 72, 'r', del_pair);
    insert_map_intchar(&map, 5, 'c', del_pair);
    insert_map_intchar(&map, 5, 'd', del_pair);
    insert_map_intchar(&map, 20, 'e', del_pair);
    insert_map_intchar(&map, 23652, 'a', del_pair);
    insert_map_intchar(&map, 0, 'g', del_pair);

    printf("Size - %zu, load factor - %lf, bucket_count - %zu\n", map.size, ((double)map.size) / map.bucket_count, map.buckets.size);

    struct node_ml_intchar* it = map.data.begin->next;

    while (it != NULL)
    {
        printf("%ld %c\n", it->data.pair.first, it->data.pair.second);
        it = it->next;
    }

    printf("\n");

    struct pair_intchar *px = find_map_intchar(&map, 20);
    printf("%ld %c\n\n", px->first, px->second);

    remove_map_intchar(&map, 0, del_pair);
    
    it = map.data.begin->next;

    while (it != NULL)
    {
        printf("%ld %c\n", it->data.pair.first, it->data.pair.second);
        it = it->next;
    }

    destroy_map_intchar(&map);
    return 0;
}