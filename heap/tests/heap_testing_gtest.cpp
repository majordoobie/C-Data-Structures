#include <gtest/gtest.h>

extern "C"
{
    #include <heap.h>
}

typedef struct payload_t
{
    int key;
    int value;
} heap_payload_t;

heap_payload_t * create_heap_payload(int key, int value)
{
    heap_payload_t * payload = (heap_payload_t *)malloc(sizeof(heap_payload_t));
    payload->key = key;
    payload->value = value;
    return payload;
}

heap_compare_t heap_compare(heap_payload_t * payload, heap_payload_t * payload2)
{
    if (payload->key > payload2->key)
    {
        return HEAP_GT;
    }
    else if (payload->key < payload2->key)
    {
        return HEAP_LT;
    }
    else
    {
        return HEAP_EQ;
    }
}

void heap_destroy(heap_payload_t * payload)
{
    free(payload);
}

class HeapTestFixture :public ::testing::Test
{
 public:
    heap_t * max_heap;
    heap_t * min_heap;
    int highest_value = 19;
    int lowest_value = 2;

 protected:
    void SetUp() override
    {
        max_heap = init_heap(heap_compare, heap_destroy, MAX_HEAP);

        insert_heap(max_heap, create_heap_payload(5, 0));
        insert_heap(max_heap, create_heap_payload(19, 0));
        insert_heap(max_heap, create_heap_payload(6, 0));
        insert_heap(max_heap, create_heap_payload(2, 0));
        insert_heap(max_heap, create_heap_payload(2, 0));
        insert_heap(max_heap, create_heap_payload(6, 0));

        min_heap = init_heap(heap_compare, heap_destroy, MIN_HEAP);
        insert_heap(min_heap, create_heap_payload(5, 0));
        insert_heap(min_heap, create_heap_payload(19, 0));
        insert_heap(min_heap, create_heap_payload(6, 0));
        insert_heap(min_heap, create_heap_payload(2, 0));
        insert_heap(min_heap, create_heap_payload(2, 0));
        insert_heap(min_heap, create_heap_payload(6, 0));
    }
    void TearDown() override
    {
        destroy_heap(max_heap);
        destroy_heap(min_heap);
    }
};

TEST_F(HeapTestFixture, TestPopValueForMax)
{
    // The highest value should be stored on the next node for max_heap
    // while lowest should be stored for a min heap
    payload_t * highest = pop_heap(max_heap);
    payload_t * lowest = pop_heap(min_heap);

    EXPECT_EQ(highest->key, highest_value);
    EXPECT_EQ(lowest->key, lowest_value);

    heap_destroy(highest);
    heap_destroy(lowest);
}