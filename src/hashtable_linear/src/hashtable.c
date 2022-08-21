#include <hashtable.h>

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INITIAL_CAPACITY 16
#define GROWTH_CAPACITY 4
#define PERTURB_SHIFT 5
// The below values are from the algorithm specifications
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1_hash
#define FNV_OFFSET 0xCBF29CE484222325
#define FNV_PRIME 0x00000100000001B3


typedef struct hashtable_iter_t
{
    const char * key;     // key is NULL if this slot is empty
    void * value;
} hashtable_entry_t;

typedef struct hashtable_t
{
    hashtable_entry_t * entries;        // hash slots
    size_t capacity;                    // size of _entries array
    size_t length;                      // number of items in hash table
    void (* free_func)(void * value);   // Optional callback to free the values
} hashtable_t;

static uint64_t hash_key(const char * key);
static bool hashtable_expand(hashtable_t* table);
static void * hashtable_set_entry(hashtable_entry_t* entries, size_t capacity,
                                  const char* key, void* value, size_t* plength);


/*!
 * @brief Instantiate a hash table object
 * @param free_func Optional pointer used to free the values passed in
 * @return Pointer to the hashtable object
 */
hashtable_t * hashtable_create(void (free_func(void * value)))
{
    hashtable_t * table = (hashtable_t *)malloc(sizeof(hashtable_t));
    if (table == NULL)
    {
        return NULL;
    }
    * table = (hashtable_t){
        .free_func  = free_func,
        .length     = 0,
        .capacity   = INITIAL_CAPACITY
    };

    table->entries = (hashtable_entry_t *)calloc(table->capacity,
                                                 sizeof(hashtable_entry_t));
    if (table->entries == NULL)
    {
        free(table);
        return NULL;
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
void hashtable_destroy(hashtable_t * table, htable_flags free_values)
{
    for (size_t index = 0; index < table->capacity; index++)
    {
        hashtable_entry_t entry = table->entries[index];

        if ((NULL != table->free_func) && (HT_FREE_VALUES_TRUE == free_values))
        {
            table->free_func(entry.value);
        }
        free((void *)entry.key);
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
bool hashtable_key_exists(hashtable_t * table, const char * key)
{
    if (NULL == hashtable_get(table, key))
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
void * hashtable_get(hashtable_t * table, const char * key)
{
    // AND hash with capacity-1 to ensure it's within entries array.
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(table->capacity - 1));

    // Loop till we find an empty entry.
    while (table->entries[index].key != NULL)
    {
        if (strcmp(key, table->entries[index].key) == 0)
        {
            // Found key, return value.
            return table->entries[index].value;
        }
        // Key wasn't in this slot, move to next (linear probing).
        index++;
        if (index >= table->capacity)
        {
            // At end of entries array, wrap around.
            index = 0;
        }
    }
    return NULL;
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
void * hashtable_set(hashtable_t* table, const char * key, void * value)
{
    assert(value != NULL);
    if (value == NULL)
    {
        return NULL;
    }

    // If length will exceed half of current capacity, expand it.
    if (table->length >= table->capacity / 2)
    {
        if (!hashtable_expand(table))
        {
            return NULL;
        }
    }

    // Set entry and update length.
    return hashtable_set_entry(table->entries,
                               table->capacity,
                               key,
                               value,
                        &table->length);
}

/*!
 * @brief Return the amount of items in the hashtable
 * @param table
 * @return
 */
size_t hashtable_length(hashtable_t * table)
{
    return table->length;
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
 * @brief Set the entry value and return the old one if there was a value there
 * @param entries
 * @param capacity
 * @param key
 * @param value
 * @param plength
 * @return Returns the pointer to the value. This is useful for when replacing
 * values with new ones and needing a way to free the old value replaced.
 */
static void * hashtable_set_entry(hashtable_entry_t * entries,
                                  size_t capacity,
                                  const char * key,
                                  void * value,
                                  size_t * plength)
{
    // AND hash with capacity-1 to ensure it's within entries array.
    uint64_t hash = hash_key(key);
    size_t slot = (size_t)(hash & (uint64_t)(capacity - 1));
    uint64_t perturb = hash;

    // Loop till we find an empty entry.
    while (entries[slot].key != NULL)
    {
        if (strcmp(key, entries[slot].key) == 0)
        {
            // Found key (it already exists), update value.
            void * old_value = entries[slot].value;
            entries[slot].value = value;
            return old_value;
        }
        perturb >>= PERTURB_SHIFT;
        slot = (PERTURB_SHIFT * slot) + 1 + perturb;
        slot = slot % capacity - 1;
    }

    // Didn't find key, allocate+copy if needed, then insert it.
    if (plength != NULL)
    {
        key = strdup(key);
        if (key == NULL)
        {
            return NULL;
        }
        (*plength)++;
    }
    void * old_value = entries[slot].value;
    entries[slot].key = (char*)key;
    entries[slot].value = value;
    return old_value;
}

// Expand hash table to twice its current size. Return true on success,
// false if out of memory.
static bool hashtable_expand(hashtable_t * table)
{
    /*
     *  used*2 + capacity/2
     */
    // Allocate new entries array.
    size_t new_capacity = table->capacity * GROWTH_CAPACITY;
    if (new_capacity < table->capacity)
    {
        return false;  // overflow (capacity would be too big)
    }
    hashtable_entry_t * new_entries = calloc(new_capacity, sizeof(hashtable_entry_t));
    if (new_entries == NULL)
    {
        return false;
    }

    // Iterate entries, move all non-empty ones to new table's entries.
    for (size_t i = 0; i < table->capacity; i++)
    {
        hashtable_entry_t entry = table->entries[i];
        if (entry.key != NULL)
        {
            hashtable_set_entry(new_entries,
                                new_capacity,
                                entry.key,
                                entry.value,
                                NULL);
        }
    }

    // Free old entries array and update this table's details.
    free(table->entries);
    table->entries = new_entries;
    table->capacity = new_capacity;
    return true;
}
