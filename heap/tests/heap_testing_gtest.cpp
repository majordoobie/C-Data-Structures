#include <gtest/gtest.h>
#include <heap.h>

void print_test(void * payload)
{
    printf("%d\n", *(int *)payload);
}


// Mandatory function for comparing the data mode payloads
heap_compare_t array_compare(void * payload, void * payload2)
{
    int val1 = *(int*)payload;
    int val2 = *(int*)payload2;

    if (val1 > val2)
    {
        return HEAP_GT;
    }
    else if (val1 < val2)
    {
        return HEAP_LT;
    }
    else
    {
        return HEAP_EQ;
    }
}

// Mandatory function for comparing the ptr payloads
heap_compare_t heap_compare(void * payload, void * payload2)
{
    int payload_1 = *(int *)payload;
    int payload_2 = *(int *)payload2;

    if (payload_1 > payload_2)
    {
        return HEAP_GT;
    }
    else if (payload_1 < payload_2)
    {
        return HEAP_LT;
    }
    else
    {
        return HEAP_EQ;
    }
}

int * create_heap_payload(int value)
{
    int * payload = (int *)malloc(sizeof(value));
    *payload = value;
    return payload;
}

void payload_destroy(void * payload)
{
    free(payload);
}

int * get_int_array(const int item_array[], size_t item_count)
{
    int * new_array = (int *)calloc(item_count, sizeof(int));
    for (size_t i = 0; i < item_count; i++)
    {
        new_array[i] = item_array[i];
    }
    return new_array;
}


class HeapTestFixture :public ::testing::Test
{
 public:
    heap_t * max_heap = nullptr;
    heap_t * min_heap = nullptr;
    heap_t * data_heap = nullptr;
    int highest_value = 16;
    int lowest_value = 2;

 protected:
    void SetUp() override
    {
        max_heap = heap_init(MAX_HEAP,
                             HEAP_PTR,
                             0,
                             payload_destroy,
                             heap_compare);
        ASSERT_NE(max_heap, nullptr);

        heap_insert(max_heap, create_heap_payload(16));
        heap_insert(max_heap, create_heap_payload(14));
        heap_insert(max_heap, create_heap_payload(10));
        heap_insert(max_heap, create_heap_payload(8));
        heap_insert(max_heap, create_heap_payload(7));
        heap_insert(max_heap, create_heap_payload(9));
        heap_insert(max_heap, create_heap_payload(3));
        heap_insert(max_heap, create_heap_payload(2));
        heap_insert(max_heap, create_heap_payload(4));
        heap_insert(max_heap, create_heap_payload(1));

        min_heap = heap_init(MIN_HEAP,
                             HEAP_PTR,
                             0,
                             payload_destroy,
                             heap_compare);
        ASSERT_NE(min_heap, nullptr);
        heap_insert(min_heap, create_heap_payload(5));
        heap_insert(min_heap, create_heap_payload(19));
        heap_insert(min_heap, create_heap_payload(6));
        heap_insert(min_heap, create_heap_payload(2));
        heap_insert(min_heap, create_heap_payload(2));
        heap_insert(min_heap, create_heap_payload(6));

        int array_count = 3;
        int my_array[] = {5, 3, 2};
        data_heap = heap_init(MAX_HEAP,
                              HEAP_MEM,
                              sizeof(int),
                              payload_destroy,
                              heap_compare);
        for (int i = 0; i < array_count; i++)
        {
            heap_insert(data_heap, &my_array[i]);
        }

        ASSERT_NE(data_heap, nullptr);

    }
    void TearDown() override
    {
        heap_destroy(max_heap);
        heap_destroy(min_heap);
        heap_destroy(data_heap);
    }
};

TEST_F(HeapTestFixture, PrintFixtures)
{
    heap_print(max_heap, print_test);

}
TEST_F(HeapTestFixture, TestPopValueForMax)
{
    // The highest value should be stored on the next node for max_heap
    // while lowest should be stored for a min heap
    void * highest = heap_pop(max_heap);
    void * lowest = heap_pop(min_heap);

    EXPECT_EQ(*(int *)highest, highest_value);
    EXPECT_EQ(*(int *)lowest, lowest_value);

    payload_destroy(highest);
    payload_destroy(lowest);
}

TEST_F(HeapTestFixture, TestAllPop)
{
    int * payload;
    while (!heap_is_empty(max_heap))
    {
        payload = (int*)heap_pop(max_heap);
        printf("-> %d\n", *payload);
        payload_destroy(payload);
    }
    printf("\n\n");
    while (!heap_is_empty(min_heap))
    {
        payload = (int*)heap_pop(min_heap);
        printf("-> %d\n", *payload);
        payload_destroy(payload);
    }
}

TEST_F(HeapTestFixture, DumpHeap)
{
    heap_dump(max_heap);
    EXPECT_TRUE(heap_is_empty(max_heap));
}



//TEST(HeapSort, HeapSortTest)
//{
//    int array_count = 3;
//    int my_array[] = {5, 3, 2};
//    int * array = get_int_array(my_array,  array_count);
//
//    heap_sort((void**)array, array_count, array_compare, MAX_HEAP);
//
//    for (int i = 0; i < array_count; i++)
//    {
//        printf("xx-> %d\n", array[i]);
//    }
//    free(array);
//}