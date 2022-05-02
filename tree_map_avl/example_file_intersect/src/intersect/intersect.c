#include <intersect.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <wctype.h>
#include <stdio.h>
#include <file_parser.h>

typedef struct
{
    key_val_t ** reduce_list;
    int size;
    int length;
    uint32_t target;
} reduce_t;

/*!
 * @brief Function creates a intersect_t object which is just a dictionary with a file
 * counter
 * @return intersection_t
 */
intersect_t * create_intersect_dict()
{
    intersect_t * intersect = calloc(1, sizeof(* intersect));
    intersect->dict = init_dict(intersect_dict_compare, intersect_free_payload, reduce_search);
    return intersect;
}

/*!
 * @brief Function is used to eliminate the the nodes from the tree that did not make it
 * to the second round. For example, if file one added 5 words and file 2 only matched
 * on 3 words, then the 2 words from the first iteration are removed.
 * @param node_payload[in] key_val_t
 * @param ptr Null pointer should fetch a reduce_t pointer
 * @return Return a recurse_t for the recursive function from the inner BST
 */
bst_recurse_t reduce_search(key_val_t * node_payload, void * ptr)
{
    reduce_t * list = (reduce_t *) ptr;
    if (node_payload->file_count == list->target)
    {
        return RECURSE_TRUE;
    }
    else
    {
        list->reduce_list[list->length] = create_intersect_payload(node_payload->original, 0);
        list->length++;
        return RECURSE_TRUE;
    }
}

/*!
 * @brief Checks the tree to see what nodes fail the iteration. If they fail, add it to
 * list of nodes to get deleted. Afterwards, iterate over the nodes and delete them.
 * @param dict[in] dict_t
 * @param iteration Number of iteration or files parsed
 */
void intersect_reduce(dict_t * dict, int iteration)
{
    reduce_t * reduce_list = calloc(1, sizeof(* reduce_list));
    reduce_list->size = get_size(dict);

    reduce_list->reduce_list = calloc(reduce_list->size, sizeof(key_val_t *));
    reduce_list->target = iteration;

    // iterate to find all the nodes we need to delete
    contains_value(dict, reduce_list);

    for (int i = 0; i < reduce_list->length; i++)
    {
        remove_key_val(dict, reduce_list->reduce_list[i]);
        intersect_free_payload(reduce_list->reduce_list[i]);
    }

    free(reduce_list->reduce_list);
    free(reduce_list);
}

/*!
 * @brief Print the root node, the call the variation print to print any children
 * @param payload[in] key_val_t
 * @param ptr Pointer is not used in this instance
 * @return
 */
bst_recurse_t intersect_print(key_val_t * payload, void * ptr)
{
    setlocale(LC_ALL, "en_US.UTF-8");
    file_tracker_t * tracker = (file_tracker_t *) ptr;

    wprintf(L"%ls ", payload->original);


    if (NULL != tracker)
    {
        if (tracker->print_all)
        {
            recurse_dict(payload->variations, variation_print, payload);
        }
    }
    wprintf(L"\n");

    return RECURSE_TRUE;
}

/*!
 * @brief This function over writes the default behaviour of the bst_insert this makes
 * it possible to avoid inserting nodes that we do not need like repeated nodes or adding
 * the children of variations of the string output
 * @param payload[in] key_val_t
 * @param ptr[in] Potentially a key_val_t
 * @param ptr2[in] Potentially a intersect_T
 * @return Status indicating if the node was inserted/updated/ignored
 */
bst_status_t intersect_adv_add(key_val_t * payload, void * ptr, void * ptr2)
{
    key_val_t * new_payload = (key_val_t *) ptr;
    intersect_t * intersect = (intersect_t *) ptr2;

    // if this hits, then this is a new node
    if (NULL == new_payload)
    {
        // this is the first iteration, so add everything
        if (intersect->file_iteration == 1)
        {
            return BST_INSERT_SUCCESS;

        }
        else
        {
            // free the payload if it will not be added
            intersect_free_payload(payload);
            int val = 1;
            return BST_INSERT_FAILURE;
        }
    }
    else
    {
        // if this hits, then we have already recorded a word for this file
        if (payload->file_count == new_payload->file_count)
        {
            intersect_free_payload(new_payload);
            return BST_INSERT_FAILURE;
        }
        else
        {
            //TODO: ERROR IS HERE - creates a verification and adds it I can see it in memory
            // but is new_payload lost?

            // increment the filecout on the string to indicate that "this file" has found
            // a match string
            payload->file_count = new_payload->file_count;
            // set the new key variation to the variation dictionary

            put_key_val(payload->variations, variation_create(new_payload->original));
            intersect_free_payload(new_payload);
            return BST_INSERT_SUCCESS;
        }
    }
}


/*!
 * @brief Creates the main nodes that go into the tree map. These will store the first
 * matched word and any other variations of it if there is a match
 * @param word
 * @param file_count
 * @return
 */
key_val_t * create_intersect_payload(const wchar_t * word, uint32_t file_count)
{
    if (NULL == word)
    {
        return NULL;
    }
    freopen(NULL, "wb", stdout);

    // Set locale before doing allocations
    setlocale(LC_ALL, "en_US.UTF-8");

    // create the key_val_t payload struct
    key_val_t * payload = calloc(1, sizeof(key_val_t));
    payload->file_count = file_count;

    // allocate space for the original string and copy it in
    size_t word_count = (wcslen(word) + 1) * sizeof(wchar_t);
    payload->original = malloc(word_count);
    wchar_t * all_caps = malloc(word_count);

    // string copy word into the two new addresses
    wcscpy(payload->original, word);
    wcscpy(all_caps, word);

    // capitalize the original word
    for (size_t i = 0; all_caps[i] != '\0'; i++)
    {
        all_caps[i] = towupper(all_caps[i]);
    }

    // transform the string to the collation byte-array for faster comparisons
    size_t transform_length = wcsxfrm(NULL, all_caps, 0) + 1;
    payload->collation_value = malloc(transform_length * sizeof(wchar_t));
    wcsxfrm(payload->collation_value, all_caps, transform_length);

    // set a slightly modified dict
    //TODO: CHECK HERE
    if (1 == file_count)
    {
        payload->variations = init_dict(intersect_dict_compare, variation_free_payload, NULL);

        put_key_val(payload->variations, variation_create(payload->original));
    }

    free(all_caps);
    return payload;
}

/*!
 * @brief Free intersect nodes
 * @param payload
 */
void intersect_free_payload(key_val_t * payload)
{
    if (NULL != payload->variations)
    {
        destroy_dict(payload->variations);
    }
    free(payload->collation_value);
    free(payload->original);
    free(payload);

}



bst_compare_t intersect_dict_compare(key_val_t * current_payload, key_val_t * new_payload)
{
    // reset collate just in case
    setlocale(LC_COLLATE, "en_US.UTF-8");

    int result = wcscmp(current_payload->collation_value, new_payload->collation_value);

    if (0 == result)
    {
        return BST_EQ;
    }
    else if (result > 0)
    {
        return BST_LT;
    }
    else
    {
        return BST_GT;
    }
}


/*!
 * @brief Create a variation key_val. The only difference from a intersection key_val is
 * that the collation key is not casted to uppercase, it maintains its original string
 * @param word[in] Word read in
 * @return Return a created payload
 */
key_val_t * variation_create(const wchar_t * word)
{
    setlocale(LC_ALL, "en_US.UTF-8");
    key_val_t * payload = calloc(1, sizeof(* payload));

    // allocate space for the original string and copy it in
    payload->original = malloc((wcslen(word) + 1) * sizeof(wchar_t));
    payload->original = wcscpy(payload->original, word);

    // transform the string to the collation byte-array for faster comparisons
    size_t transform_length = wcsxfrm(NULL, word, 0) + 1;
    payload->collation_value = malloc(transform_length * sizeof(wchar_t));
    wcsxfrm(payload->collation_value, word, transform_length);

    return payload;
}

/*!
 * @brief Free variation paylooads
 * @param payload
 */
void variation_free_payload(key_val_t * payload)
{
    free(payload->collation_value);
    free(payload->original);
    free(payload);
}

/*!
 * @brief Prints any variation strings to the terminal
 * @param payload
 * @param ptr
 * @return
 */
bst_recurse_t variation_print(key_val_t * payload, void * ptr)
{
    setlocale(LC_ALL, "en_US.UTF-8");

    key_val_t * root = (key_val_t *) ptr;
    if (0 == wcscoll(payload->original, root->original))
    {
        return RECURSE_TRUE;
    }

    wprintf(L"%ls ", payload->original);
    return RECURSE_TRUE;
}
