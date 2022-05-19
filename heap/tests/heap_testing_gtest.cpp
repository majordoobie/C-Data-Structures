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

void print_test(heap_payload_t * payload)
{
    printf("%d\n", payload->key);
}

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

void destroy_ext_heap(heap_payload_t * payload)
{
    free(payload);
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
        max_heap = heap_init(heap_compare, destroy_ext_heap, MAX_HEAP);

        heap_insert(max_heap, create_heap_payload(16, 0));
        heap_insert(max_heap, create_heap_payload(14, 0));
        heap_insert(max_heap, create_heap_payload(10, 0));
        heap_insert(max_heap, create_heap_payload(8, 0));
        heap_insert(max_heap, create_heap_payload(7, 0));
        heap_insert(max_heap, create_heap_payload(9, 0));
        heap_insert(max_heap, create_heap_payload(3, 0));
        heap_insert(max_heap, create_heap_payload(2, 0));
        heap_insert(max_heap, create_heap_payload(4, 0));
        heap_insert(max_heap, create_heap_payload(1, 0));

        min_heap = heap_init(heap_compare, destroy_ext_heap, MIN_HEAP);
        heap_insert(min_heap, create_heap_payload(5, 0));
        heap_insert(min_heap, create_heap_payload(19, 0));
        heap_insert(min_heap, create_heap_payload(6, 0));
        heap_insert(min_heap, create_heap_payload(2, 0));
        heap_insert(min_heap, create_heap_payload(2, 0));
        heap_insert(min_heap, create_heap_payload(6, 0));
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
    payload_t * highest = heap_pop(max_heap);
    payload_t * lowest = heap_pop(min_heap);

    EXPECT_EQ(highest->key, highest_value);
    EXPECT_EQ(lowest->key, lowest_value);

    destroy_ext_heap(highest);
    destroy_ext_heap(lowest);
}

TEST_F(HeapTestFixture, TestAllPop)
{
    payload_t * payload;
    while (!heap_is_empty(max_heap))
    {
        payload = heap_pop(max_heap);
        printf("-> %d\n", payload->key);
        destroy_ext_heap(payload);
    }
    printf("\n\n");
    while (!heap_is_empty(min_heap))
    {
        payload = heap_pop(min_heap);
        printf("-> %d\n", payload->key);
        destroy_ext_heap(payload);
    }
}