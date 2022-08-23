#ifndef DATA_STRUCTURES_C_HASHTABLE_SRC_HASHTABLE_H_
#define DATA_STRUCTURES_C_HASHTABLE_SRC_HASHTABLE_H_

#ifdef __cplusplus
extern "C" {

#endif // __cplusplus
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define HASH_INIT_VAL 0xCBF29CE484222325


typedef struct htable_entry_t
{
    const char * key;
    void * value;
    size_t key_size;
} htable_entry_t;

typedef enum htable_flags
{
    HT_FREE_VALUES_TRUE,
    HT_FREE_VALUES_FALSE
} htable_flags;

typedef enum htable_match_t
{
    HT_MATCH_TRUE,
    HT_MATCH_FALSE,
} htable_match_t;


typedef struct htable_t htable_t;
typedef struct htable_iter_t htable_iter_t;

// Function is used to hash the key. This should be called from the
// hashing function callback. So the callback the user provides is going to
// turn their data into a stream of bytes, then that stream is going to be
// passed to this function to be hashed.
//
// Api->hash_callback -> hash_callback -> htable-hash_key
void htable_hash_key(uint64_t * hash, void * key, size_t key_length);

// Create and destroy the hashtable. Must provide a function that can
// hash the data, a function to free the key and a function to free the
// values. The freeing is of course optional
htable_t * htable_create(
    uint64_t (* hash_callback)(void * key, size_t key_size),
    void (* free_key_callback)(void * key),
    void (* free_value_callback)(void * value),
    htable_match_t (* compare_callback)(void * left_key, size_t left_key_size,
                                        void * right_key, size_t right_key_size)
                                        );

void htable_destroy(htable_t * table, htable_flags free_values);


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
