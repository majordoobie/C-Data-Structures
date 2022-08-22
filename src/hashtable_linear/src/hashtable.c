#include <hashtable.h>

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INITIAL_CAPACITY 16
#define PERTURB_SHIFT 5

// The below values are from the algorithm specifications
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1_hash
#define FNV_OFFSET 0xCBF29CE484222325
#define FNV_PRIME 0x00000100000001B3

static const char * DUMMY_KEY = "001DummyKey";

// Enum for determining if malloc calls were valid
typedef enum
{
    VALID_PTR = 1,
    INVALID_PTR = 0
} valid_ptr_t;


typedef struct htable_t
{
    htable_entry_t ** entries;        // hash slots
    size_t capacity;                    // size of entries
    size_t slots_used;                  // number of slots used by keys
    size_t slots_filled;                // Number of slots filled by keys or dummies
    void (* free_func)(void * value);   // Optional callback to free the values
} htable_t;

typedef struct htable_iter_t
{
    size_t index;
    htable_t * table;
} htable_iter_t;

valid_ptr_t verify_alloc(void * ptr);
static uint64_t hash_key(const char * key);
static bool htable_expand(htable_t* table);
static htable_entry_t * get_entry(htable_t * table, const char * key);
static void * htable_set_entry(htable_entry_t ** entries,
                               const char * key,
                               void * value,
                               size_t * slots_used,
                               size_t * slots_filled,
                               size_t capacity);

/*!
 * @brief Return the amount of items in the hashtable
 * @param table Pointer to table structure
 * @return Number of slots used by keys
 */
size_t htable_get_length(htable_t * table)
{
    assert(table);
    return table->slots_used;
}
/*!
 * @brief Return the amount of items in the hashtable used up by keys or
 * dummy keys. Dummy keys are keys that have been removed by htable_del
 * @param table Pointer to table structure
 * @return Number of slots used by keys or dummy keys
 */
size_t htable_get_slots(htable_t * table)
{
    assert(table);
    return table->slots_filled;
}
/*!
 * @brief Instantiate a hash table object
 * @param free_func Optional pointer used to free the values passed in
 * @return Pointer to the hashtable object
 */
htable_t * htable_create(void (free_func(void * value)))
{
    htable_t * table = (htable_t *)malloc(sizeof(htable_t));
    if (INVALID_PTR == verify_alloc(table))
    {
        return NULL;
    }

    * table = (htable_t){
        .free_func    = free_func,
        .slots_used   = 0,
        .slots_filled = 0,
        .capacity     = INITIAL_CAPACITY
    };

    table->entries = (htable_entry_t **)calloc(table->capacity,
                                               sizeof(htable_entry_t *));
    if (INVALID_PTR == verify_alloc(table->entries))
    {
        return NULL;
    }
    for (size_t i = 0; i < INITIAL_CAPACITY; i++)
    {
        table->entries[i] = (htable_entry_t *)calloc(1, sizeof(htable_entry_t));
        if (INVALID_PTR == table->entries[i])
        {
            return NULL;
        }
    }

    return table;
}

/*!
 * @brief Destroy the hash table with the option of also freeing the values
 * set in the hashtable. A function to free the hashtable values must be
 * set when instantiating the hashtable.
 * @param table Pointer to the hashtable object
 * @param free_values Flag indicating if the values of the hashtable should also
 * be freed using the function set during instantiation.
 */
void htable_destroy(htable_t * table, htable_flags free_values)
{
    assert(table);
    for (size_t index = 0; index < table->capacity; index++)
    {
        htable_entry_t * entry = table->entries[index];

        if ((NULL != table->free_func) && (HT_FREE_VALUES_TRUE == free_values))
        {
            table->free_func(entry->value);
        }
        free((void *)entry->key);
        free(entry);
    }

    free(table->entries);
    free(table);
}


/*!
 * @brief Wrapper for querying for the key to see if the key exists
 * @param table Pointer to the hashtable structure
 * @param key Pointer to the key passed in. (Should not be allocated)
 * @return Bool indicating if the key exists in the table
 */
bool htable_key_exists(htable_t * table, const char * key)
{
    if (NULL == htable_get(table, key))
    {
        return false;
    }
    return true;
}

/*!
 * @brief Fetch the value from the hashtable if it exists. If it does not exist
 * return false.
 * @param table Pointer to the hashtable structure
 * @param key Pointer to the key passed in (Should not be allocated)
 * @return Pointer to the value if found or NULL if not found
 */
void * htable_get(htable_t * table, const char * key)
{
    assert(table);
    htable_entry_t * entry = get_entry(table, key);
    if (NULL != entry)
    {
        return entry->value;
    }
    return NULL;
}

void * htable_del(htable_t * table, const char * key)
{
    htable_entry_t * entry = get_entry(table, key);
    if (NULL == entry)
    {
        return NULL;
    }

    void * value = entry->value;
    entry->value = NULL;
    free((char *)entry->key);
    entry->key = strdup(DUMMY_KEY);

    // Only subtract the slots used not filled since the slot is not going to
    // be used again
    table->slots_used--;

    return value;
}



/*!
 * @brief Set value into the hashtable. The function will allocate the key for
 * you so do not pass in a allocated string or it will leak.
 * @param table Pointer to the hashtable structure
 * @param key Pointer to the key passed in (Should not be allocated)
 * @param value Pointer to the value to store in the hashtable
 * @return Returns the pointer to the value. This is useful for when replacing
 * values with new ones and needing a way to free the old value replaced.
 */
void * htable_set(htable_t* table, const char * key, void * value)
{
    assert(value != NULL);
    assert(table);

    // Expand table if we exceed the half way mark
    if (table->slots_used >= table->capacity / 2)
    {
        if (!htable_expand(table))
        {
            return NULL;
        }
    }

    return htable_set_entry(table->entries,
                            key,
                            value,
                            & table->slots_used,
                            & table->slots_filled,
                            table->capacity);

}

htable_iter_t * htable_get_iter(htable_t * table)
{
    assert(table);
    htable_iter_t * iter = (htable_iter_t *)calloc(1, sizeof(htable_iter_t));
    if (INVALID_PTR == iter)
    {
        return NULL;
    }

    iter->table = table;
    return iter;
}
void htable_destroy_iter(htable_iter_t * iter)
{
    assert(iter);
    free(iter);
}

htable_entry_t * htable_iter_get_entry(htable_iter_t * iter)
{
    assert(iter);
    htable_entry_t * entry = iter->table->entries[iter->index];
    if ((NULL == entry->key) || (strcmp(entry->key, DUMMY_KEY) == 0))
    {
        return htable_iter_get_next(iter);
    }
    return entry;
}

htable_entry_t * htable_iter_get_next(htable_iter_t * iter)
{
    assert(iter);
    htable_entry_t * entry = NULL;
    while (iter->index < iter->table->capacity)
    {
        entry = iter->table->entries[iter->index];
        if (NULL != entry)
        {
            if (strcmp(entry->key, DUMMY_KEY) != 0)
            {
                return entry;
            }
        }
        iter->index++;
    }
    return NULL;

}

/*!
 * @brief Run the key through Fowler–Noll–Vo (FNV-1a variant)
 * hash function returning a 64 bit hash. The hash uses a static FNV_OFFSET
 * and FNV_PRIME for the calculations
 * @param key Pointer to the key passed in for the look up
 * @return Return a 64 bit hash
 */
static uint64_t hash_key(const char * key)
{
    uint64_t hash = FNV_OFFSET;
    for (const char * byte = key; '\0' != *byte; byte++)
    {
        // Algorithm XOR's the key with the hash then the data is multiplied
        // by the FNV prime
        hash ^= (uint64_t)(unsigned char)(* byte);
        hash *= FNV_PRIME;
    }
    return hash;
}


/*!
 * @brief Expand the hashtable array using Pythons strategy of slots_used * 2
 * plus half the capasity
 * @param table Pointer to the table structure
 * @return True indicating that the expansion was a success
 */
static bool htable_expand(htable_t * table)
{
    // Allocate new entries array.
    size_t new_capacity = (table->slots_used * 2) + (table->capacity / 2);
    if (new_capacity < table->capacity)
    {
        return false;  // overflow (capacity would be too big)
    }
    htable_entry_t ** new_entries = (htable_entry_t **)calloc(new_capacity, sizeof(htable_entry_t *));
    if (INVALID_PTR == verify_alloc(new_entries))
    {
        return false;
    }

    for (size_t i = 0; i < new_capacity; i++)
    {
        new_entries[i] = (htable_entry_t *)calloc(1, sizeof(htable_entry_t));
        if (INVALID_PTR == verify_alloc(new_entries[i]))
        {
            return false;
        }
    }

    // Reset slot usage
    table->slots_used = 0;
    table->slots_filled = 0;

    // Iterate entries, move all non-empty ones to new table's entries.
    for (size_t i = 0; i < table->capacity; i++)
    {
        htable_entry_t * entry = table->entries[i];
        if (entry->key != NULL)
        {
            // Do not expand the dummy keys
            if (strcmp(entry->key, DUMMY_KEY) != 0)
            {
                htable_set_entry(new_entries,
                                 entry->key,
                                 entry->value,
                                 & table->slots_used,
                                 & table->slots_filled,
                                 table->capacity);
            }

            free((char *)entry->key);
        }
        free(entry);
    }

    // Free old entries array and update this table's details.
    free(table->entries);
    table->entries = new_entries;
    table->capacity = new_capacity;
    return true;
}

static htable_entry_t * get_entry(htable_t * table, const char * key)
{
    // AND the hash and capacity so that it fits within the range of slots
    // this is similar to doing a mod
    uint64_t hash = hash_key(key);
    size_t slot = (size_t)(hash & (uint64_t)(table->capacity - 1));
    uint64_t perturb = hash;

    while (table->entries[slot]->key != NULL)
    {
        if (strcmp(key, table->entries[slot]->key) == 0)
        {
            return table->entries[slot];
        }

        // se Python style probing to get the next slot where the key
        // could be in
        perturb >>= PERTURB_SHIFT;
        slot = (PERTURB_SHIFT * slot) + 1 + perturb;
        slot = slot % table->capacity - 1;
    }

    return NULL;
}

/*!
 * @brief Check if allocation is valid
 * @param ptr Any pointer
 * @return valid_ptr_t : VALID_PTR or INVALID_PTR
 */
valid_ptr_t verify_alloc(void * ptr)
{
    if (NULL == ptr)
    {
        fprintf(stderr, "[!] Invalid allocation\n");
        return INVALID_PTR;
    }
    return VALID_PTR;
}
/*!
 * @brief Set value into the hashtable. The function will allocate the key for
 * you so do not pass in a allocated string or it will leak.
 * @param table Pointer to the hashtable structure
 * @param key Pointer to the key passed in (Should not be allocated)
 * @param value Pointer to the value to store in the hashtable
 * @return Returns the pointer to the value. This is useful for when replacing
 * values with new ones and needing a way to free the old value replaced.
 */
void * htable_set_entry(htable_entry_t ** entries,
                        const char * key,
                        void * value,
                        size_t * slots_used,
                        size_t * slots_filled,
                        size_t capacity)
{
    assert(value != NULL);
    if (value == NULL)
    {
        return NULL;
    }


    // AND hash with capacity-1 to ensure it's within entries array.
    uint64_t hash = hash_key(key);
    size_t slot = (size_t)(hash & (uint64_t)(capacity - 1));
    uint64_t perturb = hash;

    // Loop till we find an empty entry.
    while (entries[slot]->key != NULL)
    {
        if (strcmp(key, entries[slot]->key) == 0)
        {
            // If key is found, update the value of it and return
            // the old value
            void * old_value = entries[slot]->value;
            entries[slot]->value = value;
            return old_value;
        }
        // Perform the python style probing
        perturb >>= PERTURB_SHIFT;
        slot = (PERTURB_SHIFT * slot) + 1 + perturb;
        slot = slot % capacity - 1;
    }


    key = strdup(key);
    if (INVALID_PTR == verify_alloc((void *)key))
    {
        return NULL;
    }
    // Update the number of slots used up by keys
    (*slots_used)++;
    (*slots_filled)++;

    void * old_value = entries[slot]->value;
    entries[slot]->key = (char*)key;
    entries[slot]->value = value;
    return old_value;
}
