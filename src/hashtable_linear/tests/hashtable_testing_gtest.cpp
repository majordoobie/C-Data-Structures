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
    hashtable_t * dict;
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
        dict = hashtable_create(free_payload);
        for (auto& key: keys)
        {
            hashtable_set(dict, key.c_str(), get_payload(key));
        }
    }
    void TearDown() override
    {
        hashtable_destroy(dict, HT_FREE_VALUES_TRUE);
    }

};


// Test that our keys are being populated
TEST_F(HashtableGtest, TestAllocAndDestroy)
{
    EXPECT_EQ(hashtable_length(this->dict), this->keys.size());
}

// Test that we can check that a key exists in the hashtable already
// Then test that we can safely fetch for a key that does not exist
TEST_F(HashtableGtest, TestKeyExists)
{
    EXPECT_EQ(true, hashtable_key_exists(this->dict, this->keys.at(0).c_str()));
    EXPECT_EQ(false, hashtable_key_exists(this->dict, "UnknownKey"));
    EXPECT_EQ(nullptr, hashtable_get(this->dict, "Unknownkey"));
}

// Test that updating a key will yield the value that was stored so that
// we can free it
TEST_F(HashtableGtest, TestValueFreeOnUpdate)
{
    std::string first_key = this->keys.at(0);
    char * new_payload = get_payload(first_key.c_str());
    EXPECT_EQ(true, hashtable_key_exists(this->dict, this->keys.at(0).c_str()));

    void * old_payload = hashtable_set(this->dict, first_key.c_str(), new_payload);

    EXPECT_EQ(true, hashtable_key_exists(this->dict, this->keys.at(0).c_str()));
    free(old_payload);
}


