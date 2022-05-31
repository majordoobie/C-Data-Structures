#include <gtest/gtest.h>
#include <heap.h>

/*
 * Heap structure supports printing your data by passing a callback to a
 * print function
 */
void print_test(void * payload)
{
    printf("%d\n", * (int *)payload);
}

/*
 * Compare function for DATA mode
 */
heap_compare_t heap_data_cmp(void * payload, void * payload2)
{
    int val1 = * (int32_t *)payload;
    int val2 = * (int32_t *)payload2;

    if (val1 > val2)
    {
        return HEAP_GT;
    } else if (val1 < val2)
    {
        return HEAP_LT;
    } else
    {
        return HEAP_EQ;
    }
}

/*
 * Compare function for PTR mode
 */
heap_compare_t heap_ptr_cmp(void * payload, void * payload2)
{
    int payload_1 = * (int *)payload;
    int payload_2 = * (int *)payload2;

    if (payload_1 > payload_2)
    {
        return HEAP_GT;
    } else if (payload_1 < payload_2)
    {
        return HEAP_LT;
    } else
    {
        return HEAP_EQ;
    }
}

int * create_heap_payload(int value)
{
    int * payload = (int *)malloc(sizeof(value));
    * payload = value;
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

class HeapTestFixture : public ::testing::Test
{
 public:
    heap_t * max_heap_ptr = nullptr;
    heap_t * min_heap_ptr = nullptr;
    heap_t * max_heap_data = nullptr;
    heap_t * min_heap_data = nullptr;

    std::vector<int32_t> test_values = {1, 3, 4, 5, 7, 8, 9, 10, 14, 16, 20,
                                        23, 34, 56, 78, 90, 100, 121, 144};

    int get_min()
    {
        return test_values.front();
    }
    int get_max()
    {
        return test_values.back();
    }

 protected:
    void SetUp() override
    {
        // Create the two ptr mode heaps and insert the values into it
        max_heap_ptr = heap_init(MAX_HEAP,
                                 HEAP_PTR,
                                 0,
                                 payload_destroy,
                                 heap_ptr_cmp);

        min_heap_ptr = heap_init(MIN_HEAP,
                                 HEAP_PTR,
                                 0,
                                 payload_destroy,
                                 heap_ptr_cmp);

        max_heap_data = heap_init(MAX_HEAP,
                                  HEAP_MEM,
                                  sizeof(int32_t),
                                  nullptr,
                                  heap_data_cmp
        );
        min_heap_data = heap_init(MIN_HEAP,
                                  HEAP_MEM,
                                  sizeof(int32_t),
                                  nullptr,
                                  heap_data_cmp
        );
        ASSERT_NE(max_heap_ptr, nullptr);
        ASSERT_NE(min_heap_ptr, nullptr);
        ASSERT_NE(min_heap_data, nullptr);
        ASSERT_NE(max_heap_data, nullptr);

        for (const int32_t &num: test_values)
        {
            heap_insert(max_heap_ptr, create_heap_payload(num));
            heap_insert(min_heap_ptr, create_heap_payload(num));
            heap_insert(min_heap_data, (void *)& num);
            heap_insert(max_heap_data, (void *)& num);
        }

    }
    void TearDown() override
    {
        heap_destroy(max_heap_ptr);
        heap_destroy(min_heap_ptr);
        heap_destroy(max_heap_data);
        heap_destroy(min_heap_data);
    }
};

//// Test that the printing callback works. This is a visual test
//TEST_F(HeapTestFixture, PrintFixtures)
//{
//    heap_print(max_heap_ptr, print_test);
//}

// Ensure that popping will present the highest, or least value depending
// on the heap type.
TEST_F(HeapTestFixture, TestPopValueForMax)
{
    // The highest value should be stored on the next node for max_heap_ptr
    // while lowest should be stored for a min heap
    void * highest = heap_pop(max_heap_ptr);
    void * lowest = heap_pop(min_heap_ptr);

    EXPECT_EQ(* (int *)highest, get_max());
    EXPECT_EQ(* (int *)lowest, get_min());

    payload_destroy(highest);
    payload_destroy(lowest);

    highest = heap_pop(max_heap_data);
    lowest = heap_pop(min_heap_data);
    EXPECT_EQ(* (int *)highest, get_max());
    EXPECT_EQ(* (int *)lowest, get_min());

    payload_destroy(highest);
    payload_destroy(lowest);
}

// Ensure that popping all elements does not cause a crash
// While at it, make sure that the order is correct
TEST_F(HeapTestFixture, TestAllPopAndOrder)
{
    int * ptr_payload;
    int count = 0;
    while (!heap_is_empty(max_heap_ptr))
    {
        ptr_payload = (int *)heap_pop(min_heap_ptr);
        EXPECT_EQ(* ptr_payload, test_values[count]);
        payload_destroy(ptr_payload);

        ptr_payload = (int *)heap_pop(max_heap_ptr);
        EXPECT_EQ(* ptr_payload, test_values[(test_values.size() - 1) - count]);
        payload_destroy(ptr_payload);

        ptr_payload = (int *)heap_pop(min_heap_data);
        EXPECT_EQ(* ptr_payload, test_values[count]);
        payload_destroy(ptr_payload);

        ptr_payload = (int *)heap_pop(max_heap_data);
        EXPECT_EQ(* ptr_payload, test_values[(test_values.size() - 1) - count]);
        payload_destroy(ptr_payload);

        count++;
    }
}

// take a dump
TEST_F(HeapTestFixture, DumpHeap)
{
    heap_dump(max_heap_ptr);
    EXPECT_TRUE(heap_is_empty(max_heap_ptr));
}

//Test the ability to have a user pass an array of continues block of memory
// and sort it based on the compare function that they pass.
TEST(HeapSort, HeapSortTestMemModeMin)
{
    int array_length = 10;
    int my_array[] = {5, 8, 2, 8, 9, 2, 3, 40, 1, 78};
    int my_array_order[] = {1, 2, 2, 3, 5, 8, 8, 9, 40, 78};
    heap_sort(&my_array, array_length, sizeof(int), HEAP_MEM, MIN_HEAP,
              heap_ptr_cmp);

    for (int i = 0; i < array_length; i++)
    {
        EXPECT_EQ(my_array[i], my_array_order[i]);
    }
}
//Test the ability to have a user pass an array of continues block of memory
// and sort it based on the compare function that they pass.
TEST(HeapSort, HeapSortTestMemModeMax)
{
    int array_length = 10;
    int my_array[] = {5, 8, 2, 8, 9, 2, 3, 40, 1, 78};
    int my_array_order[] = {1, 2, 2, 3, 5, 8, 8, 9, 40, 78};
    heap_sort(&my_array, array_length, sizeof(int), HEAP_MEM, MAX_HEAP,
              heap_ptr_cmp);

    for (int i = 0; i < array_length; i++)
    {
        EXPECT_EQ(my_array[i], my_array_order[array_length - ( 1 + i)]);
    }
}

TEST(HeapSort, HeapSortTestPtrModeMin)
{
    int array_length = 10;
    int my_array[] = {5, 8, 2, 8, 9, 2, 3, 40, 1, 78};
    int my_array_order[] = {1, 2, 2, 3, 5, 8, 8, 9, 40, 78};

    int ** int_ptr_array = (int **)calloc(array_length, sizeof(void *));
    for (int i = 0; i < array_length; i++)
    {
        int_ptr_array[i] = create_heap_payload(my_array[i]);
    }

    heap_sort(int_ptr_array, array_length, 0, HEAP_PTR, MIN_HEAP, heap_ptr_cmp);

    for (int i = 0; i < array_length; i++)
    {
        EXPECT_EQ(*int_ptr_array[i], my_array_order[i]);
        free(int_ptr_array[i]);
    }
    free(int_ptr_array);
}

TEST(HeapSort, HeapSortTestPtrModeMax)
{
    int array_length = 10;
    int my_array[] = {5, 8, 2, 8, 9, 2, 3, 40, 1, 78};
    int my_array_order[] = {1, 2, 2, 3, 5, 8, 8, 9, 40, 78};

    int ** int_ptr_array = (int **)calloc(array_length, sizeof(void *));
    for (int i = 0; i < array_length; i++)
    {
        int_ptr_array[i] = create_heap_payload(my_array[i]);
    }

    heap_sort(int_ptr_array, array_length, 0, HEAP_PTR, MAX_HEAP, heap_ptr_cmp);

    for (int i = 0; i < array_length; i++)
    {
        EXPECT_EQ(*int_ptr_array[i], my_array_order[array_length - (i + 1)]);
        free(int_ptr_array[i]);
    }
    free(int_ptr_array);
}

TEST(HeapSort, HeapSortOneMode)
{
    int array_length = 10;
    int my_array[] = {5, 8, 2, 8, 9, 2, 3, 40, 1, 78};
//    int my_array_order[] = {1, 2, 2, 3, 5, 8, 8, 9, 40, 78};
    int ** int_array = (int**)calloc(array_length, sizeof(int *));

    for (int i = 0; i < array_length; i++)
    {
        int_array[i] = create_heap_payload(my_array[i]);
    }
    test_func(int_array, sizeof(int *), array_length);


    for (int i = 0; i < array_length; i++)
    {
        free(int_array[i]);
    }
    free(int_array);
}