#include <gtest/gtest.h>
#include <dl_list.h>


/*
 * Helper Functions for testing
 */
// Creates a simple payload for the linked list
char * get_payload(int num)
{
    const int size = 20;
    char buff[size];
    snprintf(buff, size, "Hello world: %d\n", num);
    return strdup(buff);
}

// frees the payload inserted into the linked list
void free_payload(void * data)
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


/*
 * //end of Helper Functions for testing
 */


/*
 * Basic tests to get up and running
 */
TEST(dlist_test, InitTest)
{
    dlist_t * dlist = dlist_init(0);
    ASSERT_NE(dlist, nullptr);
    dlist_destroy(dlist);
}

TEST(dlist_test, InsertTest)
{
    dlist_t * dlist = dlist_init(0);
    ASSERT_NE(dlist, nullptr);
    dlist_append(dlist, get_payload(1));

    dlist_destroy_free(dlist, free_payload);
}

TEST(dlist_test, IterableTest)
{
    dlist_t * dlist = dlist_init(0);
    ASSERT_NE(dlist, nullptr);
    dlist_append(dlist, get_payload(1));
    dlist_append(dlist, get_payload(2));
    dlist_append(dlist, get_payload(3));

    dlist_iter_t * iter = dlist_get_iterable(dlist);
    char * node = (char*)dlist_get_iter_next(iter);
    while (nullptr != node)
    {
        printf("%s", node);
        node = (char*)dlist_get_iter_next(iter);

    }
    dlist_destroy_iter(iter);
    dlist_destroy_free(dlist, free_payload);
}




/*
 * Test fixture to do more complicated testing
 *
 * This fixture creates a dlist of 10 items with 10 strings
 */
class DListTestFixture : public ::testing::Test
{
 public:
    dlist_t * dlist;
    dlist_iter_t * iter;
    void * payload_first;
    void * payload_last;
    int length = 10;

 protected:
    void SetUp() override
    {
        dlist = dlist_init(compare_payloads);
        for (int i = 0; i < length; i++)
        {
            void * payload = get_payload(i);
            if (0 == i)
            {
                payload_first = payload;
            }
            else if (length - 1 == i)
            {
                payload_last = payload;
            }
            dlist_append(dlist, payload);
        }
        iter = dlist_get_iterable(dlist);
    }
    void TearDown() override
    {
        dlist_destroy_iter(iter);
        dlist_destroy_free(dlist, free_payload);
    }
};

// Test ability to pop from the tail
TEST_F(DListTestFixture, TestPopTail)
{
    char * value = (char *)dlist_pop_tail(dlist);
    ASSERT_NE(value, nullptr);

    EXPECT_EQ(value, payload_last);
    free(value);
}
// Test ability to pop from the head
TEST_F(DListTestFixture, TestPopHead)
{
    char * value = (char *)dlist_pop_head(dlist);
    ASSERT_NE(value, nullptr);

    EXPECT_EQ(value, payload_first);
    free(value);
}

// Test ability to find a match
TEST_F(DListTestFixture, TestFindInDlist)
{
    EXPECT_EQ(dlist_in_dlist(dlist, payload_last), DLIST_MATCH);

    void * no_match_payload = get_payload(-1);
    EXPECT_NE(dlist_in_dlist(dlist, no_match_payload), DLIST_MATCH);
    free(no_match_payload);
}

