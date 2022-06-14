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

