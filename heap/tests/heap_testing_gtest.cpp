#include <gtest/gtest.h>
#include <heap.h>

void print_test(void * payload)
{
    printf("%d\n", *(int *)payload);
}

/*!
 * Mandatory function to use the DST, a function that compares the
 * null pointers.
 *
 * @return Heap compare enum
 */
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

void ** get_int_array(void ** item_array, size_t  item_size, size_t
item_count)
{
    void ** new_array = (void **)calloc(item_count, sizeof(void *));
    for (size_t i = 0; i < item_count; i++)
    {
        void * item = (void *)malloc(item_size);
        memcpy(item, item_array[i], item_size);
        new_array[i] = item;
    }
    return new_array;
}






class HeapTestFixture :public ::testing::Test
{
 public:
    heap_t * max_heap;
    heap_t * min_heap;
    int highest_value = 16;
    int lowest_value = 2;

 protected:
    void SetUp() override
    {
        max_heap = heap_init(heap_compare, payload_destroy, MAX_HEAP);

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

        min_heap = heap_init(heap_compare, payload_destroy, MIN_HEAP);
        heap_insert(min_heap, create_heap_payload(5));
        heap_insert(min_heap, create_heap_payload(19));
        heap_insert(min_heap, create_heap_payload(6));
        heap_insert(min_heap, create_heap_payload(2));
        heap_insert(min_heap, create_heap_payload(2));
        heap_insert(min_heap, create_heap_payload(6));
    }
    void TearDown() override
    {
        heap_destroy(max_heap);
        heap_destroy(min_heap);
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