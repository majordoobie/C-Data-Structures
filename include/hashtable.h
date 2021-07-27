#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <check.h>
#include <stdio.h>

typedef struct hash_table
{
    int key;
}hash_table_t;

hash_table_t * init_hash_table(int key);

#endif //HASHTABLE_H
