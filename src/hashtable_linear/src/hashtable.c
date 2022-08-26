#include <hashtable.h>

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INITIAL_CAPACITY 8
#define PERTURB_SHIFT 5

// The below values are from the algorithm specifications
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1_hash
#define FNV_PRIME 0x00000100000001B3
#define FNV_OFFSET_BASIS 0xCBF29CE484222325

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
    void (* free_value)(void * value);   // Optional callback to free the values
    void (* free_key)(void * key);   // Optional callback to free the values
    uint64_t (* hash_callback)(void * key);
    htable_match_t (* compare_callback)(void * left_key, void * right_key);

} htable_t;

typedef struct htable_iter_t
{
    size_t index;
    htable_t * table;
} htable_iter_t;

static bool expand(htable_t * table);
static valid_ptr_t verify_alloc(void * ptr);
static htable_entry_t * get_entry(htable_t * table,
                                  void * key,
                                  uint64_t * slot);
static void * set_entry(htable_t * table,
                        void * key,
                        void * value);
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
 * @brief Run the key through Fowler–Noll–Vo (FNV-1a variant)
 * hash function returning a 64 bit hash. The hash uses a static HASH_INIT_VAL
 * and FNV_PRIME for the calculations. To call this function first call the
 * hash init function and pass the hash provided to this function.
 *
 * This pattern allows you to hash multiple objects like the members of a
 * struct. It is not recommended to hash a struct since the padding could
 * lead to access of unallocated memory space. Instead hashing multiple members
 * is recommended.
 * @param key Pointer to the key passed in for the look up
 * @param key_length The length of the key data pointer to iterate over it
 * @return Return a 64 bit hash
 */
void htable_hash_key(uint64_t * hash, void * key, size_t key_length)
{

    size_t position = 0;
    uint8_t * bytes = key;

    while (position < key_length)
    {
        uint8_t byte = bytes[position];
        *hash ^= byte;
        *hash ^= FNV_PRIME;
        position++;
    }
}

/*!
 * @brief Function returns the FNV offset for the FNV hashing algorithm. This
 * function should be called first to initialize the hash then pass then
 * hash X number of times to the `htable_hash_key` function to hash the objects.
 * @return FNV offset basis
 */
uint64_t htable_get_init_hash(void)
{
    return (uint64_t)FNV_OFFSET_BASIS;
}

/*!
 * @brief Initialize the hashtable object with all its callbacks.
 * @param hash_callback Mandatory callback to to hash the keys. The callback
 * should be making calls to the htable_hash_key function
 * @param compare_callback Mandatory callback to compare keys. This is used
 * when a hash collision is hit
 * @param free_key_callback Optional callback to free the keys
 * @param free_value_callback Optional callback to free the values
 * @return Pointer to allocated hashtable object
 */
htable_t * htable_create(uint64_t (* hash_callback)(void *),
                         htable_match_t (* compare_callback)(void *, void *),
                         void (* free_key_callback)(void *),
                         void (* free_value_callback)(void *))
{
    if ((NULL == hash_callback) || (NULL == compare_callback))
    {
        fprintf(stderr, "[!] Mandatory pointers to hash_callback"
                        "and compare_callback are required.\n");
        return NULL;
    }

    htable_t * table = (htable_t *)malloc(sizeof(htable_t));
    if (INVALID_PTR == verify_alloc(table))
    {
        return NULL;
    }


    * table = (htable_t){
        .free_value         = free_value_callback,
        .free_key           = free_key_callback,
        .compare_callback   = compare_callback,
        .hash_callback      = hash_callback,
        .slots_used         = 0,
        .slots_filled       = 0,
        .capacity           = INITIAL_CAPACITY
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
        table->entries[i]->_dummy_key = false;
        if (INVALID_PTR == table->entries[i])
        {
            return NULL;
        }
    }

    return table;
}

/*!
 * @brief Free the hashtable object with the option to also free the keys
 * and values
 * @param table Pointer to the hashtable object
 * @param free_keys Flag indicating if each key should be freed
 * @param free_values Flag indicating if each valued should be freed
 */
void htable_destroy(htable_t * table,
                    htable_flag_t free_keys,
                    htable_flag_t free_values)
{
    assert(table);
    for (size_t index = 0; index < table->capacity; index++)
    {
        htable_entry_t * entry = table->entries[index];
        if (NULL != entry->key)
        {
            if ((NULL != table->free_value) && (HT_FREE_PTR_TRUE == free_values))
            {
                table->free_value(entry->value);
            }

            if ((NULL != table->free_key) && (HT_FREE_PTR_TRUE == free_keys))
            {
                table->free_key(entry->key);
            }
        }

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
bool htable_key_exists(htable_t * table, void * key)
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
 * @param key Pointer to the key
 * @return Pointer to the value if found or NULL if not found
 */
void * htable_get(htable_t * table, void * key)
{
    assert(table);
    uint64_t slot = 0;
    htable_entry_t * entry = get_entry(table, key, &slot);
    if (NULL != entry)
    {
        return entry->value;
    }
    return NULL;
}

/*!
 * @brief Remove a key value pair from the hashtable and return the value if
 * found. Additionally, free the key if htable_flat_t is set to true.
 *
 * The key will then be set to dummy. This will keep the slot from being
 * used again until an expansion is called. This reduces the time complexity
 * of removing keys. Same technique used by Pythons dictionaries since it's
 * very unlikely that a hashtable gets their items removed.
 *
 * The tradeoff is to maintain the space complexity to obtain a higher time
 * complexity by keeping the same size array when items are removed.
 *
 * @param table Pointer to the hashtable object
 * @param key Pointer to the key
 * @param free_key Flag indicating if the key should be freed
 * @return Pointer to the value if found else NULL if not found
 */
void * htable_del(htable_t * table, void * key, htable_flag_t free_key)
{
    assert(table);

    uint64_t slot = 0;
    htable_entry_t * entry = get_entry(table, key, &slot);
    if (NULL == entry)
    {
        return NULL;
    }

    void * value = entry->value;
    entry->value = NULL;

    // Set key to be a dummy, meaning that the slot will not be freed
    entry->_dummy_key = true;

    if (HT_FREE_PTR_TRUE == free_key)
    {
        free(entry->key);
    }
    entry->key = NULL;

    // Only subtract the slots used not filled since the
    // slot is not going to be used again
    table->slots_used--;

    return value;
}

/*!
 * @brief Set value into the hashtable. The function will allocate the key for
 * you so do not pass in a allocated string or it will leak.
 * @param table Pointer to the hashtable structure
 * @param key Pointer to the key
 * @param value Pointer to the value to store in the hashtable
 * @return Returns the pointer to the value. This is useful for when replacing
 * values with new ones and needing a way to free the old value replaced.
 */
void * htable_set(htable_t * table, void * key, void * value)
{
    assert(value != NULL);
    assert(table);

    // Expand table if we exceed the halfway mark
    if (table->slots_used >= table->capacity / 2)
    {
        if (!expand(table))
        {
            return NULL;
        }
    }

    return set_entry(table, key, value);
}


/*!
 * @brief Create an iter object
 * @param table Pointer to the hashtable object
 * @return Pointer to the iter object
 */
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

/*!
 * @brief Destroy the iter object
 * @param iter Pointer to the iter object
 */
void htable_destroy_iter(htable_iter_t * iter)
{
    assert(iter);
    free(iter);
}

/*!
 * @brief Get the entry pointer from
 * @param iter Pointer to the iterable object
 * @return The next available entry or NULL if the end of the array is reached
 */
htable_entry_t * htable_iter_get_entry(htable_iter_t * iter)
{
    assert(iter);
    htable_entry_t * entry = iter->table->entries[iter->index];
    if ((NULL == entry->key) || (false == entry->_dummy_key))
    {
        return htable_iter_get_next(iter);
    }
    return entry;
}

/*!
 * @brief Iterate the iter object to point to the next value slot in the array
 * @param iter Pointer to the iter object
 * @return Next entry in the array or NULL if the end of the array is reached
 */
htable_entry_t * htable_iter_get_next(htable_iter_t * iter)
{
    assert(iter);
    htable_entry_t * entry = NULL;
    while (iter->index < iter->table->capacity)
    {
        entry = iter->table->entries[iter->index];
        iter->index++;
        if (NULL != entry)
        {
            if (NULL != entry->key)
            {
                return entry;
            }
        }
    }
    return NULL;

}


/*!
 * @brief Expand the array using pythons strategy of
 * (slots_used * 2) + (capacity / 2)
 * @param table Pointer to the table structure
 * @return True indicating that the expansion was a success
 */
static bool expand(htable_t * table)
{
    // Allocate new entries array.
    size_t new_capacity = (table->slots_used * 2) + (table->capacity / 2);
    if (new_capacity < table->capacity)
    {
        return false;
    }
    htable_entry_t ** new_entries =
        (htable_entry_t **)calloc(new_capacity, sizeof(htable_entry_t *));
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
        new_entries[i]->_dummy_key = false;
    }

    // Reset slot usage
    table->slots_used = 0;
    table->slots_filled = 0;

    // Update table with new objects
    htable_entry_t ** old_entries = table->entries;
    size_t old_capacity = table->capacity;
    table->entries = new_entries;
    table->capacity = new_capacity;

    // Iterate entries, move all non-empty ones to new table's entries.
    for (size_t i = 0; i < old_capacity; i++)
    {
        htable_entry_t * entry = old_entries[i];
        if (entry->key != NULL)
        {
            // Do not expand the dummy keys
            if (false == entry->_dummy_key)
            {
                set_entry(table, entry->key, entry->value);
            }
        }
        free(entry);
    }

    free(old_entries);
    return true;
}

/*!
 * @brief Hash the key and use python style probing to find the key. If the
 * key is not found then return NULL;
 * @param table Pointer to the hashtable object
 * @param key Pointer to the key object
 * @return Hash table entry object if found or NULL if not found
 */
static htable_entry_t * get_entry(htable_t * table,
                                  void * key,
                                  uint64_t * slot)
{

    // AND the hash and capacity so that it fits within the range of slots
    // this is similar to doing a mod
    uint64_t hash = table->hash_callback(key);
    *slot = hash % (table->capacity - 1);
    uint64_t perturb = hash;

    htable_entry_t * current_entry = table->entries[(*slot)];

    /*
     * Index each slot from the algorithm to find a none NULL key. If a
     * NULL is found, but the entry is set to dummy, perform a probe and
     * try again until the MATCH is found or a NULL entry with a dummy of false
     * is found.
     */
    while ((NULL != current_entry->key) || (true == current_entry->_dummy_key))
    {
        if (false == current_entry->_dummy_key)
        {
            if (HT_MATCH_TRUE == (table->compare_callback(key, table->entries[(*slot)]->key)))
            {
                return table->entries[(*slot)];
            }
        }


        // use Python style probing to get the next slot where
        // the key could be in
        perturb >>= PERTURB_SHIFT;
        *slot = (PERTURB_SHIFT * (*slot)) + 1 + perturb;
        *slot = (*slot) % (table->capacity - 1);
        current_entry = table->entries[(*slot)];
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
static void * set_entry(htable_t * table, void * key, void * value)
{
    assert(value != NULL);
    if (value == NULL)
    {
        return NULL;
    }

    uint64_t slot = 0;
    htable_entry_t * entry = get_entry(table, key, &slot);
    if (NULL != entry)
    {
        void * old_value = entry->value;
        entry->value = value;
        return old_value;
    }


    // Update the number of slots used up by keys
    table->slots_used++;
    table->slots_filled++;

    void * old_value = table->entries[slot]->value;
    table->entries[slot]->key = key;
    table->entries[slot]->value = value;
    table->entries[slot]->_dummy_key = false;

    return old_value;
}

/*!
 * @brief Check if allocation is valid
 * @param ptr Any pointer
 * @return valid_ptr_t : VALID_PTR or INVALID_PTR
 */
static valid_ptr_t verify_alloc(void * ptr)
{
    if (NULL == ptr)
    {
        fprintf(stderr, "[!] Invalid allocation\n");
        return INVALID_PTR;
    }
    return VALID_PTR;
}
