#ifndef DATA_STRUCTURES_C_HASHTABLE_SRC_HASHTABLE_H_
#define DATA_STRUCTURES_C_HASHTABLE_SRC_HASHTABLE_H_

#ifdef __cplusplus
extern "C" {

#endif // __cplusplus
#include <stdbool.h>
#include <stddef.h>

// There are situations where a ptr is needed as a key instead of using a str.
// To handle both situations, the hashtable will convert the ptr address into
// a string to store it on the hashtable.
typedef enum htable_key_type
{
    HT_KEY_AS_STR,
    HT_KEY_AS_PTR
} htable_key_type;

typedef struct htable_entry_t
{
    const char * key;
    void * value;
    htable_key_type key_type;
} htable_entry_t;

typedef enum htable_flags
{
    HT_FREE_VALUES_TRUE,
    HT_FREE_VALUES_FALSE
} htable_flags;


typedef struct htable_t htable_t;
typedef struct htable_iter_t htable_iter_t;

size_t htable_get_length(htable_t * table);
htable_t * htable_create(void (free_func(void * value)));
void htable_destroy(htable_t * table, htable_flags free_values);

bool htable_key_exists(htable_t * table,
                       const char * key,
                       htable_key_type key_type);

void * htable_get(htable_t * table, const char * key, htable_key_type key_type);

void * htable_set_str(htable_t * table,
                      const char * key,
                      htable_key_type key_type,
                      void * value);

void * htable_del(htable_t * table, const char * key, htable_key_type key_type);
size_t htable_get_slots(htable_t * table);

htable_iter_t * htable_get_iter(htable_t * table);
void htable_destroy_iter(htable_iter_t * iter);
htable_entry_t * htable_iter_get_entry(htable_iter_t * iter);
htable_entry_t * htable_iter_get_next(htable_iter_t * iter);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif //DATA_STRUCTURES_C_HASHTABLE_SRC_HASHTABLE_H_
