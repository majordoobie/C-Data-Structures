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

// Hash table structure: create with hashtable_create, free with hashtable_destroy.
typedef struct hashtable_t hashtable_t;

// Hash table iterator: create with hashtable_iterator, iterate with hashtable_next.
typedef struct {
    const char * key;  // current key
    void * value;      // current value

    // Don't use these fields directly.
    hashtable_t * _table;       // reference to hash table being iterated
    size_t _index;    // current index into hashtable_t._entries
} hashtable_iter_t;

// Return number of items in hash table.
size_t hashtable_length(hashtable_t * table);

// Create hash table and return pointer to it, or NULL if out of memory.
hashtable_t * hashtable_create(void (free_func(void * value)));

// Free memory allocated for hash table, including allocated keys.
void hashtable_destroy(hashtable_t * table, htable_flags free_values);

// Get item with given key (NUL-terminated) from hash table. Return
// value (which was set with hashtable_set), or NULL if key not found.
void * hashtable_get(hashtable_t * table, const char * key);
bool hashtable_key_exists(hashtable_t * table, const char * key);

// Set item with given key (NUL-terminated) to value (which must not
// be NULL). If not already present in table, key is copied to newly
// allocated memory (keys are freed automatically when hashtable_destroy is
// called). Return address of copied key, or NULL if out of memory.
const char * hashtable_set(hashtable_t * table, const char * key, void * value);



// Return new hash table iterator (for use with hashtable_next).
hashtable_iter_t hashtable_iterator(hashtable_t * table);

// Move iterator to next item in hash table, update iterator's key
// and value to current item, and return true. If there are no more
// items, return false. Don't call hashtable_set during iteration.
bool hashtable_next(hashtable_iter_t * it);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif //DATA_STRUCTURES_C_HASHTABLE_SRC_HASHTABLE_H_
