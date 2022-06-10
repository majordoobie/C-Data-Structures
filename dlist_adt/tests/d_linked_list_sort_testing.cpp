#include <gtest/gtest.h>
#include <dl_list.h>

dlist_match_t match_int_payloads(void * left, void * right)
{
    if (*(int*)left == *(int*)right)
    {
        return DLIST_MATCH;
    }
    return DLIST_MISS_MATCH;
}

dlist_compare_t compare_int_payloads(void * left, void * right)
{
    int p_left = *(int*)left;
    int p_right = *(int*)right;
    if (p_left == p_right)
    {
        return DLIST_EQ;
    }
    return (p_left > p_right) ? DLIST_GT : DLIST_LT;
}

int * get_int_payload(int value)
{
    int * val = (int *)malloc(sizeof(int));
    *val = value;
    return val;
}

void free_int_payload(void * data)
{
    free(data);
}

class DListTestSortFixture : public ::testing::Test
{
 public:
    dlist_t * dlist{};
    dlist_t * dlist2{};

    int length = 10;
    std::vector<int> test_array2 = {5, 3, 4, 6, 19, 1, -1, 8, 3, 19};
    std::vector<int> test_array = {2, 8, 7, 1, 3, 5, 6, 4};

 protected:
    void SetUp() override
    {
        dlist = dlist_init(match_int_payloads);
        dlist2 = dlist_init(match_int_payloads);
        for (auto& v: test_array)
        {
            dlist_append(dlist, get_int_payload(v));
        }
        for (auto& v: test_array2)
        {
            dlist_append(dlist2, get_int_payload(v));
        }
    }
    void TearDown() override
    {
        dlist_destroy_free(dlist, free_int_payload);
        dlist_destroy_free(dlist2, free_int_payload);
    }
};


// Test the ability to quick sort the double linked list
TEST_F(DListTestSortFixture, SortAscending)
{
    // sort the test array
    std::sort(this->test_array.begin(), this->test_array.end());

    dlist_quick_sort(this->dlist, ASCENDING, compare_int_payloads);
    dlist_iter_t * iter = dlist_get_iterable(dlist, ITER_HEAD);

    int count = 0;
    int node = *(int *)dlist_get_iter_value(iter);

    while (count < (int)this->test_array.size() - 1)
    {
        const int& target = this->test_array.at(count);
        EXPECT_EQ(node, target);
        node = *(int*)dlist_get_iter_next(iter);
        count++;
    }

    dlist_destroy_iter(iter);
}

// Test the ability to quick sort the double linked list
TEST_F(DListTestSortFixture, SortAscendingAdv)
{
    // sort the test array
    std::sort(this->test_array2.begin(), this->test_array2.end());

    dlist_quick_sort(this->dlist2, ASCENDING, compare_int_payloads);
    dlist_iter_t * iter = dlist_get_iterable(dlist2, ITER_HEAD);

    int count = 0;
    int node = *(int *)dlist_get_iter_value(iter);

    while (count < (int)this->test_array2.size() - 1)
    {
        const int& target = this->test_array2.at(count);
        EXPECT_EQ(node, target);
        node = *(int*)dlist_get_iter_next(iter);
        count++;
    }

    dlist_destroy_iter(iter);
}

// Test ability to quick sort the double linked list
TEST_F(DListTestSortFixture, SortDescending)
{
    // sort the test array
    std::sort(this->test_array.begin(), this->test_array.end());
    sort(this->test_array.begin(), this->test_array.end(), std::greater<int>());

    dlist_quick_sort(this->dlist, DESCENDING, compare_int_payloads);
    dlist_iter_t * iter = dlist_get_iterable(dlist, ITER_HEAD);

    int count = 0;
    int node = *(int *)dlist_get_iter_value(iter);

    while (count < (int)this->test_array.size() - 1)
    {
        const int& target = this->test_array.at(count);
        EXPECT_EQ(node, target);
        node = *(int*)dlist_get_iter_next(iter);
        count++;
    }

    dlist_destroy_iter(iter);
}

// Test ability to quick sort the double linked list
TEST_F(DListTestSortFixture, SortDescendingAdv)
{
    // sort the test array
    std::sort(this->test_array2.begin(), this->test_array2.end());
    sort(this->test_array2.begin(), this->test_array2.end(), std::greater<int>
        ());

    dlist_quick_sort(this->dlist2, DESCENDING, compare_int_payloads);
    dlist_iter_t * iter = dlist_get_iterable(dlist2, ITER_HEAD);

    int count = 0;
    int node = *(int *)dlist_get_iter_value(iter);

    while (count < (int)this->test_array2.size() - 1)
    {
        const int& target = this->test_array2.at(count);
        EXPECT_EQ(node, target);
        node = *(int*)dlist_get_iter_next(iter);
        count++;
    }

    dlist_destroy_iter(iter);
}
