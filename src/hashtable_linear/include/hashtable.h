#ifndef DATA_STRUCTURES_C_HASHTABLE_SRC_HASHTABLE_H_
#define DATA_STRUCTURES_C_HASHTABLE_SRC_HASHTABLE_H_

#ifdef __cplusplus
extern "C" {

#endif // __cplusplus
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


typedef struct htable_entry_t
{
    const char * key;
    void * value;
} htable_entry_t;

typedef enum htable_flags
{
    HT_FREE_VALUES_TRUE,
    HT_FREE_VALUES_FALSE
} htable_flags;


typedef struct htable_t htable_t;
typedef struct htable_iter_t htable_iter_t;

// Create and destroy the hashtable. Must provide a function that can
// hash the data, a function to free the key and a function to free the
// values. The freeing is of course optional
htable_t * htable_create(void (free_func(void * value)));
void htable_destroy(htable_t * table, htable_flags free_values);

uint64_t htable_hash_key(void * key, size_t key_length);

size_t htable_get_length(htable_t * table);

bool htable_key_exists(htable_t * table, const char * key);

void * htable_get(htable_t * table, const char * key);

void * htable_set(htable_t * table, const char * key, void * value);

void * htable_del(htable_t * table, const char * key);
size_t htable_get_slots(htable_t * table);

htable_iter_t * htable_get_iter(htable_t * table);
void htable_destroy_iter(htable_iter_t * iter);
htable_entry_t * htable_iter_get_entry(htable_iter_t * iter);
htable_entry_t * htable_iter_get_next(htable_iter_t * iter);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif //DATA_STRUCTURES_C_HASHTABLE_SRC_HASHTABLE_H_
