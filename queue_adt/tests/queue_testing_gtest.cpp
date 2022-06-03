#include <gtest/gtest.h>
#include <dl_queue.h>

/*
 * Helper Functions for testing
 */
// Creates a simple payload for the linked list
int * get_payload(int num)
{
    int * ptr = (int * )malloc(sizeof(int));
    *ptr = num;
    return ptr;
}

// frees the payload inserted into the linked list
void free_payload(void * data)
{
    free(data);
}

// function for comparing nodes
queue_status_t compare_payloads(void * data1, void * data2)
{
    if ((int*)data1 == (int*)data2)
    {
        return Q_MATCH;
    }
    return Q_NO_MATCH;
}
/*
 * //end of Helper Functions for testing
 */

TEST(QueueTest, TestAllocation)
{
    queue_t * queue = queue_init(10, compare_payloads);
    ASSERT_NE(queue, nullptr);
    queue_destroy(queue);
}
