#include <gtest/gtest.h>
#include <hashtable.h>

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

    uint64_t first_hash = HASH_INIT_VAL;
    uint64_t second_hash = HASH_INIT_VAL;

    // Test hashing the same string
    htable_hash_key(&first_hash, payload1, strlen(payload1));
    htable_hash_key(&second_hash, payload1, strlen(payload1));
    EXPECT_EQ(first_hash, second_hash);


    // Let's get more complicated and hash the string with a member
    first_hash = HASH_INIT_VAL;
    second_hash = HASH_INIT_VAL;
    htable_hash_key(&first_hash, payload1, strlen(payload1));
    htable_hash_key(&second_hash, payload2->payload, strlen(payload2->payload));
    EXPECT_EQ(first_hash, second_hash);

    // Let's get even more complicated and hash the structs
    first_hash = HASH_INIT_VAL;
    second_hash = HASH_INIT_VAL;
    htable_hash_key(&first_hash, payload2->payload, strlen(payload2->payload));
    htable_hash_key(&first_hash, &payload2->val, sizeof(int));

    htable_hash_key(&second_hash, &payload3->val, sizeof(int));
    EXPECT_NE(first_hash, second_hash);

    // Let's get even more complicated and hash the structs
    first_hash = HASH_INIT_VAL;
    second_hash = HASH_INIT_VAL;
    htable_hash_key(&first_hash, payload2->payload, strlen(payload2->payload));
    htable_hash_key(&first_hash, &payload2->val, sizeof(int));
    char * payload4 = strdup("Some Payload");
    payload3->payload = payload4;
    htable_hash_key(&second_hash, payload3->payload, strlen(payload3->payload));
    htable_hash_key(&second_hash, &payload3->val, sizeof(int));
    EXPECT_EQ(first_hash, second_hash);

    // Let's get even more complicated and hash the structs
    first_hash = HASH_INIT_VAL;
    second_hash = HASH_INIT_VAL;
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

    for (int i = 0; i < count; count++)
    {
        s->list[i] = (int)i;
    }

    return s;
}

// Callback to free the payload
void free_payload(void * payload)
{
    test_struct_t * s = (test_struct_t *)payload;
    free(s->list);
    free(s->payload);
    free(s);
}


uint64_t hash_callback(void * key, size_t key_size)
{
    test_struct_t * s = (test_struct_t *)key;
    uint64_t hash = HASH_INIT_VAL;

    htable_hash_key(&hash, s->payload, strlen(s->payload));
    htable_hash_key(&hash, &s->val, sizeof(int));
    htable_hash_key(&hash, s->list, (size_t)s->val);

    return hash;
}

htable_match_t compare_callback(void * left_key, size_t left_key_size, void * right_key, size_t right_key_size)
{
    test_struct_t * left = (test_struct_t *)left_key;
    test_struct_t * right = (test_struct_t *)right_key;

    // This test function only tests the payload strings. But you would compare
    // all items of the struct
    if (0 == strcmp(left->payload, right->payload))
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
        dict = htable_create(hash_callback, free_payload, NULL, compare_callback);

        int count = 0;
        for (auto& key: keys)
        {
            test_struct_t * payload = get_payload(key.c_str(), count);
            htable_set(dict, payload, payload);
            count++;
        }
    }
    void TearDown() override
    {
        htable_destroy(dict, HT_FREE_PTR_TRUE, HT_FREE_PTR_FALSE);
    }

};


//// Test that our keys are being populated
//TEST_F(HashtableGtest, TestAllocAndDestroy)
//{
//    EXPECT_EQ(htable_get_length(this->dict), this->keys.size());
//}

//// Test that we can check that a key exists in the hashtable already
//// Then test that we can safely fetch for a key that does not exist
//TEST_F(HashtableGtest, TestKeyExists)
//{
//    EXPECT_EQ(true, htable_key_exists(this->dict,
//                                      this->keys.at(0).c_str(), 0));
//    EXPECT_EQ(true, htable_key_exists(this->dict,
//                                      this->keys.at(7).c_str(), 0));
//    EXPECT_EQ(true, htable_key_exists(this->dict,
//                                      this->keys.at(9).c_str(), 0));
//    EXPECT_EQ(true, htable_key_exists(this->dict,
//                                      this->keys.at(11).c_str(), 0));
//    EXPECT_EQ(false, htable_key_exists(this->dict, "UnknownKey", 0));
//    EXPECT_EQ(nullptr, htable_get(this->dict, "Unknownkey"));
//}

// Test that updating a key will yield the value that was stored so that
// we can free it
//TEST_F(HashtableGtest, TestValueFreeOnUpdate)
//{
//    std::string first_key = this->keys.at(0);
//    char * new_payload = get_payload(first_key.c_str());
//    EXPECT_EQ(true, htable_key_exists(this->dict,
//                                      this->keys.at(0).c_str(), 0));
//
//    void * old_payload =
//        htable_set(this->dict,
//                   first_key.c_str(),
//                   new_payload);
//
//    EXPECT_EQ(true, htable_key_exists(this->dict,
//                                      this->keys.at(0).c_str(), 0));
//    free(old_payload);
//}
//
////Test the deletion functionality
//TEST_F(HashtableGtest, TestKeyRemoval)
//{
//    EXPECT_EQ(htable_get_length(this->dict), this->keys.size());
//
//    std::string first_key = this->keys.at(0);
//    void * payload = htable_del(this->dict, first_key.c_str());
//
//    EXPECT_EQ(htable_get_length(this->dict), this->keys.size() - 1);
//    EXPECT_EQ(htable_get_slots(this->dict), this->keys.size());
//
//    free(payload);
//}
//
//// Test ability to delete all the keys
//TEST_F(HashtableGtest, TestKeyAllRemoval)
//{
//    EXPECT_EQ(htable_get_length(this->dict), this->keys.size());
//    htable_iter_t * iter = htable_get_iter(this->dict);
//
//    htable_entry_t * entry = htable_iter_get_entry(iter);
//    while (NULL != entry)
//    {
//        void * payload = htable_del(this->dict, entry->key);
//        free(payload);
//        entry = htable_iter_get_next(iter);
//    }
//
//    EXPECT_EQ(htable_get_length(this->dict), 0);
//    htable_destroy_iter(iter);
//}
//
//// Test that expansion is not coping over the removed keys
//TEST(HashtableSoloTest, TestExpansion)
//{
//    htable_t * dict = htable_create(NULL, free_payload, NULL, NULL);
//
//    // The base capacity is 8 so we can safely add 4 items before expansion
//    htable_set(dict, "Key1", get_payload("Key1"));
//    htable_set(dict, "Key2", get_payload("Key2"));
//    htable_set(dict, "Key3", get_payload("Key3"));
//    htable_set(dict, "Key4", get_payload("Key4"));
//
//    EXPECT_EQ(4, htable_get_length(dict));
//    EXPECT_EQ(4, htable_get_slots(dict));
//
//    free(htable_del(dict, "Key1"));
//
//    EXPECT_EQ(3, htable_get_length(dict));
//    EXPECT_EQ(4, htable_get_slots(dict));
//
//
//    // Our slots should still go up so it shoud not be higher than
//    // our used slots by 1
//    htable_set(dict, "Key5", get_payload("Key5"));
//    EXPECT_EQ(4, htable_get_length(dict));
//    EXPECT_EQ(5, htable_get_slots(dict));
//
//
//    // This should now trigger the resize the "removed" slots will not be
//    // copied over
//    htable_set(dict, "Key6", get_payload("Key6"));
//    EXPECT_EQ(5, htable_get_length(dict));
//    EXPECT_EQ(5, htable_get_slots(dict));
//
//    htable_destroy(dict, HT_FREE_PTR_TRUE);
//}
//
//TEST_F(HashtableGtest, TestAbilityToHashPointer)
//{
//    htable_entry_t * some_payload = (htable_entry_t *)calloc(1, sizeof(htable_entry_t));
//    some_payload->value = get_payload("Payload test");
//
//    htable_set(this->dict, (const char *)some_payload, some_payload);
//
//    htable_entry_t * fetch_payload = (htable_entry_t  *)htable_get(this->dict,
//                                                                   (const char *)some_payload);
//    EXPECT_EQ(fetch_payload->value, some_payload->value);
//
//    free(some_payload->value);
//}
//
