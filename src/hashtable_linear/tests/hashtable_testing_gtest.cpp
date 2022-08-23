#include <gtest/gtest.h>
#include <hashtable.h>

typedef struct test_struct_t
{
    char * payload;
    int val;
} test_struct_t;

TEST(HashtableSingleTest, TestHasingFunction)
{
    char * payload1 = strdup("Some Payload");
    EXPECT_EQ(htable_hash_key((void *)payload1, strlen(payload1)),
              htable_hash_key((void *)payload1, strlen(payload1)));

    EXPECT_EQ(htable_hash_key((void *)payload1, strlen(payload1)),
              htable_hash_key((void *)payload1, strlen(payload1)));

    test_struct_t * payload2 = (test_struct_t *)calloc(1, sizeof(test_struct_t));
    test_struct_t * payload3 = (test_struct_t *)calloc(1, sizeof(test_struct_t));
    payload2->payload = payload1;

    EXPECT_EQ(htable_hash_key((void *) payload2, sizeof(test_struct_t)),
              htable_hash_key((void *)payload2, sizeof(test_struct_t)));

    EXPECT_NE(htable_hash_key((void *)payload2, sizeof(test_struct_t)),
              htable_hash_key((void *)payload3, sizeof(test_struct_t)));

    EXPECT_EQ(htable_hash_key((void *)payload1, strlen(payload1)),
              htable_hash_key((void *)payload2->payload, strlen(payload2->payload)));


    free(payload1);
    free(payload2);
    free(payload3);
}

char * get_payload(std::string string)
{
    return strdup(string.c_str());
}

void free_payload(void * payload)
{
    free(payload);
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
        dict = htable_create(free_payload);
        for (auto& key: keys)
        {
            htable_set(dict, key.c_str(), get_payload(key));
        }
    }
    void TearDown() override
    {
        htable_destroy(dict, HT_FREE_VALUES_TRUE);
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
                                      this->keys.at(0).c_str(), 0));
    EXPECT_EQ(true, htable_key_exists(this->dict,
                                      this->keys.at(7).c_str(), 0));
    EXPECT_EQ(true, htable_key_exists(this->dict,
                                      this->keys.at(9).c_str(), 0));
    EXPECT_EQ(true, htable_key_exists(this->dict,
                                      this->keys.at(11).c_str(), 0));
    EXPECT_EQ(false, htable_key_exists(this->dict, "UnknownKey", 0));
    EXPECT_EQ(nullptr, htable_get(this->dict, "Unknownkey"));
}

// Test that updating a key will yield the value that was stored so that
// we can free it
TEST_F(HashtableGtest, TestValueFreeOnUpdate)
{
    std::string first_key = this->keys.at(0);
    char * new_payload = get_payload(first_key.c_str());
    EXPECT_EQ(true, htable_key_exists(this->dict,
                                      this->keys.at(0).c_str(), 0));

    void * old_payload =
        htable_set(this->dict,
                   first_key.c_str(),
                   new_payload);

    EXPECT_EQ(true, htable_key_exists(this->dict,
                                      this->keys.at(0).c_str(), 0));
    free(old_payload);
}

//Test the deletion functionality
TEST_F(HashtableGtest, TestKeyRemoval)
{
    EXPECT_EQ(htable_get_length(this->dict), this->keys.size());

    std::string first_key = this->keys.at(0);
    void * payload = htable_del(this->dict, first_key.c_str());

    EXPECT_EQ(htable_get_length(this->dict), this->keys.size() - 1);
    EXPECT_EQ(htable_get_slots(this->dict), this->keys.size());

    free(payload);
}

// Test ability to delete all the keys
TEST_F(HashtableGtest, TestKeyAllRemoval)
{
    EXPECT_EQ(htable_get_length(this->dict), this->keys.size());
    htable_iter_t * iter = htable_get_iter(this->dict);

    htable_entry_t * entry = htable_iter_get_entry(iter);
    while (NULL != entry)
    {
        void * payload = htable_del(this->dict, entry->key);
        free(payload);
        entry = htable_iter_get_next(iter);
    }

    EXPECT_EQ(htable_get_length(this->dict), 0);
    htable_destroy_iter(iter);
}

// Test that expansion is not coping over the removed keys
TEST(HashtableSoloTest, TestExpansion)
{
    htable_t * dict = htable_create(free_payload);

    // The base capacity is 8 so we can safely add 4 items before expansion
    htable_set(dict, "Key1", get_payload("Key1"));
    htable_set(dict, "Key2", get_payload("Key2"));
    htable_set(dict, "Key3", get_payload("Key3"));
    htable_set(dict, "Key4", get_payload("Key4"));

    EXPECT_EQ(4, htable_get_length(dict));
    EXPECT_EQ(4, htable_get_slots(dict));

    free(htable_del(dict, "Key1"));

    EXPECT_EQ(3, htable_get_length(dict));
    EXPECT_EQ(4, htable_get_slots(dict));


    // Our slots should still go up so it shoud not be higher than
    // our used slots by 1
    htable_set(dict, "Key5", get_payload("Key5"));
    EXPECT_EQ(4, htable_get_length(dict));
    EXPECT_EQ(5, htable_get_slots(dict));


    // This should now trigger the resize the "removed" slots will not be
    // copied over
    htable_set(dict, "Key6", get_payload("Key6"));
    EXPECT_EQ(5, htable_get_length(dict));
    EXPECT_EQ(5, htable_get_slots(dict));

    htable_destroy(dict, HT_FREE_VALUES_TRUE);
}

TEST_F(HashtableGtest, TestAbilityToHashPointer)
{
    htable_entry_t * some_payload = (htable_entry_t *)calloc(1, sizeof(htable_entry_t));
    some_payload->value = get_payload("Payload test");

    htable_set(this->dict, (const char *)some_payload, some_payload);

    htable_entry_t * fetch_payload = (htable_entry_t  *)htable_get(this->dict,
                                                                   (const char *)some_payload);
    EXPECT_EQ(fetch_payload->value, some_payload->value);

    free(some_payload->value);
}

