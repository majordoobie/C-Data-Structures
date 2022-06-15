#include <gtest/gtest.h>
#include <circular_list.h>

// Function to create payloads for testing
char * get_payload(const char * string)
{
    return strdup(string);
}

// Function to free the payloads
void free_payload(void * node)
{
    free(node);
}

// function to perform comparisons
clist_compare_t compare_payloads(void * l, void * r)
{
    char * left = (char *)l;
    char * right = (char *)r;

    int cmp = strcmp(left, right);
    if (cmp > 0)
    {
        return CLIST_GT;
    }
    else if (cmp == 0)
    {
        return CLIST_EQ;
    }
    return CLIST_LT;
}

clist_match_t match_payloads(void * l, void * r)
{
    clist_compare_t cmp = compare_payloads(l, r);
    if (CLIST_EQ == cmp)
    {
        return CLIST_MATCH;
    }
    return CLIST_MISS_MATCH;
}


class CListTestFixture : public ::testing::Test
{
 public:
    clist_t * clist;

    std::vector<std::string> words = {
        "one", "two", "Three", "four", "5", "six", "seven"
    };

 protected:
    void SetUp() override
    {
        this->clist = clist_init(this->words.size(), match_payloads, free_payload);
        for (std::string& val : this->words)
        {
            clist_insert(this->clist, get_payload(val.c_str()), 0, TAIL);
        }
    }
    void TearDown() override
    {
        clist_destroy(this->clist, FREE_NODES_TRUE);
    }
};

TEST_F(CListTestFixture, TestInit)
{
    EXPECT_EQ(this->words.size(), clist_get_length(this->clist));
}

TEST_F(CListTestFixture, TestIteration)
{
    // Get the first value from the circular linked list
    char * node = (char *)clist_get_value(this->clist);

    for (std::string& word: this->words)
    {
        EXPECT_EQ(strcmp(word.c_str(), node), 0);
        node = (char *)clist_get_next(this->clist);
    }

    // We can iterate again because the circular linked list loops around
    for (std::string& word: this->words)
    {
        EXPECT_EQ(strcmp(word.c_str(), node), 0);
        node = (char *)clist_get_next(this->clist);
    }
}


TEST_F(CListTestFixture, TestFindingNode)
{
    std::string node_to_find = this->words.at(this->words.size() / 2);
    char * target_node = (char *)clist_find(this->clist, (void *)node_to_find.c_str());

    EXPECT_EQ(strcmp(node_to_find.c_str(), target_node), 0);

    // Now find something not present

    char * new_payload = get_payload("This is a new sting not present");
    target_node = (char *)clist_find(this->clist, new_payload);

    EXPECT_EQ(target_node, nullptr);
    free_payload(new_payload);
}