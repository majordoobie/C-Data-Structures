#ifndef DATA_STRUCTURES_C_HASHTABLE_SRC_HASHTABLE_H_
#define DATA_STRUCTURES_C_HASHTABLE_SRC_HASHTABLE_H_

#ifdef __cplusplus
extern "C" {

#endif // __cplusplus
#include <stdbool.h>
#include <stddef.h>
typedef enum htable_flags
{
    HT_FREE_VALUES_TRUE,
    HT_FREE_VALUES_FALSE
} htable_flags;

typedef struct hashtable_t hashtable_t;

size_t hashtable_length(hashtable_t * table);
hashtable_t * hashtable_create(void (free_func(void * value)));
void hashtable_destroy(hashtable_t * table, htable_flags free_values);
void * hashtable_get(hashtable_t * table, const char * key);
bool hashtable_key_exists(hashtable_t * table, const char * key);
void * hashtable_set(hashtable_t * table, const char * key, void * value);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif //DATA_STRUCTURES_C_HASHTABLE_SRC_HASHTABLE_H_
