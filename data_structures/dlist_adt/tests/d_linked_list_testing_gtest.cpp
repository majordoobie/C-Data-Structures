#include <gtest/gtest.h>
#include <dl_list.h>
#include <cstring>

extern "C"
{
    extern int32_t get_inverse(int32_t value);
}

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
    EXPECT_EQ(dlist_get_length(dlist), 0);
    dlist_destroy(dlist);
}

TEST(dlist_test, InsertTest)
{
    dlist_t * dlist = dlist_init(0);
    ASSERT_NE(dlist, nullptr);
    dlist_append(dlist, get_payload(1));
    EXPECT_EQ(dlist_get_length(dlist), 1);

    dlist_destroy_free(dlist, free_payload);
}

TEST(dlist_test, IterableTest)
{
    dlist_t * dlist = dlist_init(0);
    ASSERT_NE(dlist, nullptr);
    dlist_append(dlist, get_payload(1));
    dlist_append(dlist, get_payload(2));
    dlist_append(dlist, get_payload(3));
    EXPECT_EQ(dlist_get_length(dlist), 3);

    dlist_iter_t * iter = dlist_get_iterable(dlist, ITER_HEAD);
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
    char * payload_first;
    char * payload_last;
    int length = 10;
    std::vector<std::string> test_vector;

 protected:
    void SetUp() override
    {
        dlist = dlist_init(compare_payloads);
        for (int i = 0; i < length; i++)
        {
            char * payload = get_payload(i);
            test_vector.emplace_back(payload);
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
        iter = dlist_get_iterable(dlist, ITER_HEAD);
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

    EXPECT_EQ(dlist_get_length(dlist), length - 1);

    EXPECT_EQ(value, payload_last);
    free(value);
}
// Test ability to pop from the head
TEST_F(DListTestFixture, TestPopHead)
{
    char * value = (char *)dlist_pop_head(dlist);
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(dlist_get_length(dlist), length - 1);

    EXPECT_EQ(value, payload_first);
    free(value);
}

// Test ability to find a match
TEST_F(DListTestFixture, TestFindInDlist)
{
    EXPECT_EQ(dlist_in_dlist(dlist, payload_last), DLIST_MATCH);

    void * no_match_payload = get_payload(-1);
    EXPECT_NE(dlist_in_dlist(dlist, no_match_payload), DLIST_MATCH);
    EXPECT_EQ(dlist_get_length(dlist), length);
    free(no_match_payload);
}

// Test ability to fetch an item from the linked list
TEST_F(DListTestFixture, TestGetInDList)
{
    void * to_match = get_payload(5);
    void * the_match = dlist_get_by_value(dlist, to_match);

    EXPECT_EQ(strcmp((char *)to_match, (char *)the_match), 0);

    EXPECT_EQ(dlist_get_length(dlist), length);

    free(to_match);
}

// Test ability to properly remove a value by searching for it
TEST_F(DListTestFixture, TestRemoveValue)
{
    // iter to test if this works
    dlist_iter_t * iter = dlist_get_iterable(dlist, ITER_HEAD);
    void * node;

    void * to_match = get_payload(5);
    void * the_match = dlist_remove_value(dlist, to_match);

    EXPECT_EQ(strcmp((char *)to_match, (char *)the_match), 0);
    EXPECT_EQ(dlist_get_length(dlist), length - 1);

    while (NULL != (node = dlist_get_iter_next(iter)))
    {
        printf("%s", (char*)node);
    }

    free(to_match);
    free(the_match);
    dlist_destroy_iter(iter);
}

// Test ability to prepend nodes by popping from the tail
// prepending it
TEST_F(DListTestFixture, TestPrepend)
{
    // pop from the tail
    char * value = (char *)dlist_pop_tail(dlist);
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(dlist_get_length(dlist), length - 1);
    EXPECT_EQ(value, payload_last);

    // prepend the tail value to the head
    dlist_prepend(dlist, value);
    EXPECT_EQ(dlist_get_length(dlist), length);

    // pop the head value, and it should still be the "payload_last:
    value = (char *)dlist_pop_head(dlist);
    EXPECT_EQ(dlist_get_length(dlist), length - 1);
    EXPECT_EQ(value, payload_last);

    free(value);
}

// Test that forward iteration is working properly
TEST_F(DListTestFixture, TestIterForward)
{
    void * node = dlist_get_iter_value(this->iter);
    int count = 0;
    while (count < this->length)
    {
        // get the reference to avoid copying the string value
        const std::string& target_value = this->test_vector.at(count);
        EXPECT_EQ(std::strcmp(target_value.c_str(), (const char *)node), 0)
        << "index: " << count << "\nTarget value: " << target_value.c_str() <<
        "\nExpected Value: " << (char *)node;

        // get the next node
        node = (char * )dlist_get_iter_next(this->iter);
        count++;
    }
    EXPECT_EQ(count, 10);
}

// Test the reverse iteration is working properly
TEST_F(DListTestFixture, TestIterReverse)
{
    // set the tail since our init is for head
    dlist_set_iter_tail(this->iter);

    void * node = dlist_get_iter_value(this->iter);

    int count = this->length - 1;
    while (count > -1)
    {
        // get the reference to avoid copying the string value
        const std::string& target_value = this->test_vector.at(count);
        EXPECT_EQ(std::strcmp(target_value.c_str(), (const char *)node), 0)
                        << "index: " << count << "\nTarget value: "
                        << target_value.c_str() <<
                        "\nExpected Value: " << (char *)node;

        node = (char *)dlist_get_iter_prev(this->iter);
        count--;
    }
    EXPECT_EQ(count, -1);
}

// Test ability to change the direction of the iter as we see fit
TEST_F(DListTestFixture, TestForwardReverse)
{
    void * node = dlist_get_iter_value(this->iter);
    int count = 0;
    while (count < this->length)
    {
        // get the reference to avoid copying the string value
        const std::string& target_value = this->test_vector.at(count);
        EXPECT_EQ(std::strcmp(target_value.c_str(), (const char *)node), 0);

        node = (char * )dlist_get_iter_next(this->iter);
        count++;
    }
    EXPECT_EQ(count, 10);

    // Reset to the tail
    dlist_set_iter_tail(this->iter);
    count = this->length - 1;
    node = (char * )dlist_get_iter_value(this->iter);
    while (count > -1)
    {
        // get the reference to avoid copying the string value
        const std::string& target_value = this->test_vector.at(count);
        EXPECT_EQ(std::strcmp(target_value.c_str(), (const char *)node), 0)
                        << "index: " << count << "\nTarget value: "
                        << target_value.c_str() <<
                        "\nExpected Value: " << (char *)node;
        node = (char *)dlist_get_iter_prev(this->iter);
        count--;
    }
    EXPECT_EQ(count, -1);

    // reset back to head
    dlist_set_iter_head(this->iter);
    node = (char * )dlist_get_iter_value(this->iter);

    count = 0;
    while (count < this->length)
    {
        // get the reference to avoid copying the string value
        const std::string& target_value = this->test_vector.at(count);
        EXPECT_EQ(std::strcmp(target_value.c_str(), (const char *)node), 0);

        node = (char * )dlist_get_iter_next(this->iter);
        count++;
    }

    EXPECT_EQ(count, 10);
}

// Test ability to inverse the length for iterating in reverse
TEST_F(DListTestFixture, TesteInverseFunc)
{
    // get the inverse in cpp
    int target_value = this->length;
    target_value *= -1;

    EXPECT_EQ(target_value, get_inverse(this->length));
}


// Test ability to fetch a value by its index even if it is negative
TEST_F(DListTestFixture, TestFetchByIndex)
{
    // Fetch one higher than expected
    void * data = dlist_get_by_index(this->dlist, this->length + 1);
    EXPECT_EQ(data, nullptr);

    // Get one lower than least
    data = dlist_get_by_index(this->dlist, (-this->length) - 1);
    EXPECT_EQ(data, nullptr);

    data = dlist_get_by_index(this->dlist, -1);
    EXPECT_EQ(data, this->payload_last);

    data = dlist_get_by_index(this->dlist, -(this->length));
    EXPECT_EQ(data, this->payload_first);


    data = dlist_get_by_index(this->dlist, -(this->length) + 1);
    std::string& target_val = this->test_vector.at(1);
    EXPECT_EQ(std::strcmp(target_val.c_str(), (const char *)data), 0);
}


TEST_F(DListTestFixture, TestInsertAt)
{
    // get a middle index
    int32_t middle_index = (int32_t)this->test_vector.size() / 2;
    void * middle_node = dlist_get_by_index(this->dlist, middle_index);
    dlist_remove_value(this->dlist, middle_node);

    // Now try to insert at the same location
    dlist_insert(this->dlist, middle_node, middle_index);

    // reset back to head
    char * node = (char * )dlist_get_iter_value(this->iter);
    for (std::string& val: this->test_vector)
    {
        EXPECT_EQ(std::strcmp(val.c_str(), node), 0) << val.c_str() << " " << node;
        node = (char *)dlist_get_iter_next(this->iter);
    }

}