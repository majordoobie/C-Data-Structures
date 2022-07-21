#include <gtest/gtest.h>
#include <graph_dlist.h>

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


TEST(GraphBasic, TestBasicStartUp)
{
    graph_t * graph = graph_init(compare_payloads);
    EXPECT_NE(graph, nullptr);
    graph_destroy(graph);
}