#include <gtest/gtest.h>
#include <hashtable.h>

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
            htable_set(dict, key.c_str(), HT_KEY_AS_STR, get_payload(key));
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
                                      this->keys.at(0).c_str(),
                                      HT_KEY_AS_STR));
    EXPECT_EQ(true, htable_key_exists(this->dict,
                                      this->keys.at(7).c_str(),
                                      HT_KEY_AS_STR));
    EXPECT_EQ(true, htable_key_exists(this->dict,
                                      this->keys.at(9).c_str(),
                                      HT_KEY_AS_STR));
    EXPECT_EQ(true, htable_key_exists(this->dict,
                                      this->keys.at(11).c_str(),
                                      HT_KEY_AS_STR));
    EXPECT_EQ(false, htable_key_exists(this->dict, "UnknownKey", HT_KEY_AS_STR));
    EXPECT_EQ(nullptr, htable_get(this->dict, "Unknownkey", HT_KEY_AS_STR));
}

// Test that updating a key will yield the value that was stored so that
// we can free it
TEST_F(HashtableGtest, TestValueFreeOnUpdate)
{
    std::string first_key = this->keys.at(0);
    char * new_payload = get_payload(first_key.c_str());
    EXPECT_EQ(true, htable_key_exists(this->dict,
                                      this->keys.at(0).c_str(),
                                      HT_KEY_AS_STR));

    void * old_payload =
        htable_set(this->dict,
                   first_key.c_str(),
                   HT_KEY_AS_STR,
                   new_payload);

    EXPECT_EQ(true, htable_key_exists(this->dict,
                                      this->keys.at(0).c_str(),
                                      HT_KEY_AS_STR));
    free(old_payload);
}

//Test the deletion functionality
TEST_F(HashtableGtest, TestKeyRemoval)
{
    EXPECT_EQ(htable_get_length(this->dict), this->keys.size());

    std::string first_key = this->keys.at(0);
    void * payload = htable_del(this->dict, first_key.c_str(), HT_KEY_AS_STR);

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
        void * payload = htable_del(this->dict, entry->key, HT_KEY_AS_STR);
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
    htable_set(dict, "Key1", HT_KEY_AS_STR, get_payload("Key1"));
    htable_set(dict, "Key2", HT_KEY_AS_STR, get_payload("Key2"));
    htable_set(dict, "Key3", HT_KEY_AS_STR, get_payload("Key3"));
    htable_set(dict, "Key4", HT_KEY_AS_STR, get_payload("Key4"));

    EXPECT_EQ(4, htable_get_length(dict));
    EXPECT_EQ(4, htable_get_slots(dict));

    free(htable_del(dict, "Key1", HT_KEY_AS_STR));

    EXPECT_EQ(3, htable_get_length(dict));
    EXPECT_EQ(4, htable_get_slots(dict));


    // Our slots should still go up so it shoud not be higher than
    // our used slots by 1
    htable_set(dict, "Key5", HT_KEY_AS_STR, get_payload("Key5"));
    EXPECT_EQ(4, htable_get_length(dict));
    EXPECT_EQ(5, htable_get_slots(dict));


    // This should now trigger the resize the "removed" slots will not be
    // copied over
    htable_set(dict, "Key6", HT_KEY_AS_STR, get_payload("Key6"));
    EXPECT_EQ(5, htable_get_length(dict));
    EXPECT_EQ(5, htable_get_slots(dict));

    htable_destroy(dict, HT_FREE_VALUES_TRUE);
}

TEST_F(HashtableGtest, TestAbilityToHashPointer)
{
    htable_entry_t * some_payload = (htable_entry_t *)calloc(1, sizeof(htable_entry_t));
    some_payload->value = get_payload("Payload test");

    htable_set(this->dict, (const char *)some_payload, HT_KEY_AS_PTR, some_payload);

    htable_entry_t * fetch_payload = (htable_entry_t  *)htable_get(this->dict, (const char *)some_payload, HT_KEY_AS_PTR);
    EXPECT_EQ(fetch_payload->value, some_payload->value);

    free(some_payload->value);

//    char * payload = get_payload("Some string");
//
//    size_t buffer_size = sizeof(void *) * 2 + 1;
//    char adr[sizeof(void * ) * 2 + 1] = {0}; // Note the +1 to make room for NULL terminator
//
//    int position = 0;
//    uintptr_t ptrAddress = (uintptr_t)payload;
//    for(size_t index = 0; index < sizeof(void *); ++index)
//    {
//        position += snprintf(adr + position, buffer_size, "%2.2X", (unsigned int)(unsigned char)(ptrAddress & 0xff));
//        ptrAddress >>= 8;
//    }
//    free(payload);
}

