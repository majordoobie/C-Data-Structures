#include <gtest/gtest.h>
#include <hashtable.h>
#include <dl_list.h>

typedef struct test_struct_t
{
    char * payload;
    int val;
    int * list;
} test_struct_t;

TEST(HashtableSingleTest, TestHasingFunction)
{
    char * payload1 = strdup("Some Payload");

    test_struct_t * payload2 = (test_struct_t *)calloc(1, sizeof(test_struct_t));
    test_struct_t * payload3 = (test_struct_t *)calloc(1, sizeof(test_struct_t));
    payload2->payload = payload1;

    uint64_t first_hash = htable_get_init_hash();
    uint64_t second_hash = htable_get_init_hash();

    // Test hashing the same string
    htable_hash_key(&first_hash, payload1, strlen(payload1));
    htable_hash_key(&second_hash, payload1, strlen(payload1));
    EXPECT_EQ(first_hash, second_hash);


    // Let's get more complicated and hash the string with a member
    first_hash = htable_get_init_hash();
    second_hash = htable_get_init_hash();
    htable_hash_key(&first_hash, payload1, strlen(payload1));
    htable_hash_key(&second_hash, payload2->payload, strlen(payload2->payload));
    EXPECT_EQ(first_hash, second_hash);

    // Let's get even more complicated and hash the structs
    first_hash = htable_get_init_hash();
    second_hash = htable_get_init_hash();
    htable_hash_key(&first_hash, payload2->payload, strlen(payload2->payload));
    htable_hash_key(&first_hash, &payload2->val, sizeof(int));

    htable_hash_key(&second_hash, &payload3->val, sizeof(int));
    EXPECT_NE(first_hash, second_hash);

    // Let's get even more complicated and hash the structs
    first_hash = htable_get_init_hash();
    second_hash = htable_get_init_hash();
    htable_hash_key(&first_hash, payload2->payload, strlen(payload2->payload));
    htable_hash_key(&first_hash, &payload2->val, sizeof(int));
    char * payload4 = strdup("Some Payload");
    payload3->payload = payload4;
    htable_hash_key(&second_hash, payload3->payload, strlen(payload3->payload));
    htable_hash_key(&second_hash, &payload3->val, sizeof(int));
    EXPECT_EQ(first_hash, second_hash);

    // Let's get even more complicated and hash the structs
    first_hash = htable_get_init_hash();
    second_hash = htable_get_init_hash();
    htable_hash_key(&first_hash, payload2->payload, strlen(payload2->payload));
    htable_hash_key(&first_hash, &payload2->val, sizeof(int));
    char * payload5 = strdup("some Payload");
    payload3->payload = payload5;
    htable_hash_key(&second_hash, payload3->payload, strlen(payload3->payload));
    htable_hash_key(&second_hash, &payload3->val, sizeof(int));
    EXPECT_NE(first_hash, second_hash);


    free(payload1);
    free(payload2);
    free(payload3);
    free(payload4);
    free(payload5);
}


// Test function to create payloads
test_struct_t * get_payload(const char * string, int count)
{
    test_struct_t * s = (test_struct_t *)malloc(sizeof(test_struct_t));

    s->payload = strdup(string);
    s->val = count;

    s->list = (int *)calloc((size_t)count, sizeof(int));
    for (int i = 0; i < count; i++)
    {
        s->list[i] = i;
    }

    return s;
}

// Callback to free the payload
void free_payload(void * payload)
{
    test_struct_t * s = (test_struct_t *)payload;
    free(s->payload);
    free(s->list);
    free(s);
}


uint64_t hash_callback(void * key)
{
    char * s = (char *)key;
    uint64_t hash = htable_get_init_hash();

    htable_hash_key(&hash, s, strlen(s));
    return hash;
}

htable_match_t compare_callback(void * left_key, void * right_key)
{
    char * left = (char *)left_key;
    char * right = (char *)right_key;

    // This test function only tests the payload strings. But you would compare
    // all items of the struct
    if (0 == strcmp(left, right))
    {
        return HT_MATCH_TRUE;
    }
    return HT_MATCH_FALSE;
}

class HashtableGtest : public ::testing::Test
{
 public:
    htable_t * dict;
    std::vector<std::string> keys = {
        std::string("First"),
        std::string("Second"),
        std::string("Third"),
        std::string("Fourth"),
        std::string("Fifth"),
        std::string("Sixth"),
        std::string("Seventh"),
        std::string("Eight"),
        std::string("9"),
        std::string("10"),
        std::string("11"),
        std::string("12"),
    };

 protected:
    void SetUp() override
    {
        dict = htable_create(hash_callback,
                             compare_callback,
                             NULL,
                             free_payload);

        int count = 0;
        for (auto& key: keys)
        {
            test_struct_t * payload = get_payload(key.c_str(), count);
            htable_set(dict, payload->payload, payload);
            count++;
        }
    }
    void TearDown() override
    {
        htable_destroy(dict, HT_FREE_PTR_FALSE, HT_FREE_PTR_TRUE);
    }

};


// Test that our keys are being populated
TEST_F(HashtableGtest, TestAllocAndDestroy)
{
    EXPECT_EQ(htable_get_length(this->dict), this->keys.size());
}

// Test that we can check that a key exists in the hashtable already
// Then test that we can safely fetch for a key that does not exist
TEST_F(HashtableGtest, TestKeyExists)
{
    EXPECT_EQ(true, htable_key_exists(this->dict,
                                      (void *)this->keys.at(0).c_str()));

    EXPECT_EQ(true, htable_key_exists(this->dict, (void*)this->keys.at(7).c_str()));
    EXPECT_EQ(true, htable_key_exists(this->dict, (void*)this->keys.at(9).c_str()));
    EXPECT_EQ(true, htable_key_exists(this->dict, (void*)this->keys.at(11).c_str()));

    EXPECT_EQ(true, htable_key_exists(this->dict, (void*)"First"));
    EXPECT_EQ(false, htable_key_exists(this->dict, (void*)"UnknownKey"));
    EXPECT_EQ(nullptr, htable_get(this->dict, (void*)"Unknownkey"));
}

// Test that updating a key will yield the value that was stored so that
// we can free it
TEST_F(HashtableGtest, TestValueFreeOnUpdate)
{
    std::string first_key = this->keys.at(0);
    test_struct_t * new_payload = get_payload(first_key.c_str(), 10);

    EXPECT_EQ(true, htable_key_exists(this->dict, (void *)this->keys.at(0).c_str()));

    void * old_payload = htable_set(this->dict, (void *)first_key.c_str(), new_payload);

    EXPECT_EQ(true, htable_key_exists(this->dict, (void *)this->keys.at(0).c_str()));

    free_payload(old_payload);
}

//Test the deletion functionality
TEST_F(HashtableGtest, TestKeyRemoval)
{
    EXPECT_EQ(htable_get_length(this->dict), this->keys.size());

    std::string first_key = this->keys.at(0);
    void * payload = htable_del(this->dict, (void *)first_key.c_str(), HT_FREE_PTR_FALSE);

    EXPECT_EQ(htable_get_length(this->dict), this->keys.size() - 1);
    EXPECT_EQ(htable_get_slots(this->dict), this->keys.size());

    free_payload(payload);
}

// Test ability to delete all the keys
TEST_F(HashtableGtest, TestKeyAllRemoval)
{
    EXPECT_EQ(htable_get_length(this->dict), this->keys.size());
    htable_iter_t * iter = htable_get_iter(this->dict);

    htable_entry_t * entry = htable_iter_get_entry(iter);
    while (NULL != entry)
    {
        void * payload = htable_del(this->dict, (void *)entry->key, HT_FREE_PTR_FALSE);
        free_payload(payload);
        entry = htable_iter_get_next(iter);
    }

    EXPECT_EQ(htable_get_length(this->dict), 0);
    htable_destroy_iter(iter);
}

// Test that expansion is not coping over the removed keys
TEST(HashtableSoloTest, TestExpansion)
{
    htable_t * dict = htable_create(hash_callback,
                         compare_callback,
                         NULL,
                         free_payload);

    // The base capacity is 8 so we can safely add 4 items before expansion
    htable_set(dict, (void *)"Key1", get_payload("Key1", 0));
    htable_set(dict, (void *)"Key2", get_payload("Key2", 0));
    htable_set(dict, (void *)"Key3", get_payload("Key3", 0));
    htable_set(dict, (void *)"Key4", get_payload("Key4", 0));

    EXPECT_EQ(4, htable_get_length(dict));
    EXPECT_EQ(4, htable_get_slots(dict));

    free_payload(htable_del(dict, (void *)"Key1", HT_FREE_PTR_FALSE));

    EXPECT_EQ(3, htable_get_length(dict));
    EXPECT_EQ(4, htable_get_slots(dict));


    // Our slots should still go up so it shoud not be higher than
    // our used slots by 1
    htable_set(dict, (void *)"Key5", get_payload("Key5", 0));
    EXPECT_EQ(4, htable_get_length(dict));
    EXPECT_EQ(5, htable_get_slots(dict));


    // This should now trigger the resize the "removed" slots will not be
    // copied overman
    htable_set(dict, (void *)"Key6", get_payload("Key6", 0));
    EXPECT_EQ(5, htable_get_length(dict));
    EXPECT_EQ(6, htable_get_slots(dict));

    htable_destroy(dict, HT_FREE_PTR_FALSE, HT_FREE_PTR_TRUE);
}


// frees the payload inserted into the linked list
void free_payload_dl(void * data)
{
    free(data);
}

// function for comparing nodes
dlist_match_t compare_payloads(void * data1, void * data2)
{
    if (0 == strcmp((char*)data1, (char*)data2))
    {
        return DLIST_MATCH;
    }
    return DLIST_MISS_MATCH;
}

TEST(HashtableBenchMark, BenchMark)
{
    // path to the word list file
    const char * pathname = "../../src/hashtable_linear/tests/words.txt";
    FILE * file = fopen(pathname, "r");
    bool good_path = false;
    if (NULL != file)
    {
        good_path = true;
    }

    if (!good_path)
    {

        pathname = "src/hashtable_linear/tests/words.txt";
        file = fopen(pathname, "r");
        if (NULL != file)
        {
            good_path = true;

        }
    }
    if (!good_path)
    {

        pathname = "../../../../src/hashtable_linear/tests/words.txt";
        file = fopen(pathname, "r");
        if (NULL == file)
        {
            fprintf(stderr, "Could not find the word.txt file\n");
            exit(1);
        }
        if (NULL != file)
        {
            good_path = true;
        }

    }
    if (!good_path)
    {
        return;
    }


    int buffer_size = 1024;
    char * buffer = (char *)calloc((size_t)buffer_size, sizeof(char));
    dlist_t * words = dlist_init(compare_payloads);

    while (NULL != fgets(buffer, buffer_size, file))
    {
        // Find the new line and set it to a null terminator
        buffer[strcspn(buffer, "\n")] = 0;
        char * word = strdup(buffer);
        if (NULL == word)
        {
            fprintf(stderr, "Ran out of memory\n");
            exit(1);
        }
        dlist_append(words, word);
    }

    // Close the file
    int result = fclose(file);
    if (0 != result)
    {
        perror("closing");
        exit(1);
    }

    htable_t * table = htable_create(hash_callback, compare_callback, NULL, free_payload_dl);
    dlist_iter_t * word_iter = dlist_get_iterable(words, ITER_HEAD);
    char * word = (char *)iter_get_value(word_iter);
    while (NULL != word)
    {
        int * val = (int *)calloc(1, sizeof(int));
        htable_set(table, word, val);
        word = (char *)dlist_get_iter_next(word_iter);
    }

    // Ensure that the list of 466549 words match the dictionary
    EXPECT_EQ(dlist_get_length(words), htable_get_length(table));


    // Now iterate over the words and ensure that we can pull each of the words
    int run = 0;
    int runs = 10;
    while (run < runs)
    {
        // Reset the iter back to head
        dlist_set_iter_head(word_iter);
        word = (char *)iter_get_value(word_iter);
        while (NULL != word)
        {
            int * val = (int *)htable_get(table, word);
            // Ensure that we got an actual value
            EXPECT_NE(val, nullptr);

            // Ensure that the value stored is the same as the run value
            // this proves that we are pulling the correct keys each time
            EXPECT_EQ(*val, run);

            // Increment the value to match the run value for the next run
            (*val)++;

            // Insert the value back into the dict as an update value
            htable_set(table, word, val);

            // fetch the next word
            word = (char *)dlist_get_iter_next(word_iter);
        }
        // Increment the run which should match the values stored in the table
        run++;
    }

    // Destroy iter
    dlist_destroy_iter(word_iter);

    // Free the buffer
    free(buffer);

    // Free words
    dlist_destroy_free(words, free_payload_dl);

    // Destroy table
    htable_destroy(table, HT_FREE_PTR_FALSE, HT_FREE_PTR_TRUE);
}

