#ifndef DATA_STRUCTURES_C_HASHTABLE_SRC_HASHTABLE_H_
#define DATA_STRUCTURES_C_HASHTABLE_SRC_HASHTABLE_H_

#ifdef __cplusplus
extern "C" {

#endif // __cplusplus
#include <stdbool.h>
#include <stddef.h>

typedef struct htable_entry_t
{
    const void * key;
    size_t key_length;
    void * value;
} htable_entry_t;

typedef enum htable_flags
{
    HT_FREE_VALUES_TRUE,
    HT_FREE_VALUES_FALSE
} htable_flags;

typedef struct htable_t htable_t;
typedef struct htable_iter_t htable_iter_t;

size_t htable_get_length(htable_t * table);
size_t htable_get_slots(htable_t * table);

htable_t * htable_create(void (free_func(void * value)));
void htable_destroy(htable_t * table, htable_flags free_values);

void * htable_get(htable_t * table, const void * key, size_t key_length);
bool htable_key_exists(htable_t * table, const void * key, size_t key_length);
void * htable_set(htable_t * table,
                  const void * key,
                  size_t key_length,
                  void * value);
void * htable_del(htable_t * table, const void * key, size_t key_length);

htable_iter_t * htable_get_iter(htable_t * table);
void htable_destroy_iter(htable_iter_t * iter);
htable_entry_t * htable_iter_get_entry(htable_iter_t * iter);
htable_entry_t * htable_iter_get_next(htable_iter_t * iter);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif //DATA_STRUCTURES_C_HASHTABLE_SRC_HASHTABLE_H_
