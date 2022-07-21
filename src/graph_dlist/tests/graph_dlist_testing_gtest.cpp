#include <gtest/gtest.h>
#include <graph_dlist.h>

/*
 * Helper Functions for testing
 */
// creates a payload for the graph
int * get_payload(int num)
{
    int * payload = (int *)(malloc(sizeof(int)));
    *payload = num;
    return payload;
}

// frees the payload inserted into the linked list
void free_payload(void * data)
{
    free(data);
}

// function for comparing nodes
dlist_match_t compare_payloads(void * data1, void * data2)
{
    if ((int*)data1 == (int*)data2)
    {
        return DLIST_MATCH;
    }
    return DLIST_MISS_MATCH;
}


TEST(GraphBasic, TestBasicStartUp)
{
    graph_t * graph = graph_init(GRAPH_DIRECTIONAL_FALSE, compare_payloads);
    EXPECT_NE(graph, nullptr);
    graph_destroy(graph);
}

class GraphDlistFixture : public ::testing::Test
{
 public:
    graph_t * graph{};

 protected:
    void SetUp() override
    {
        graph = graph_init(GRAPH_DIRECTIONAL_TRUE, compare_payloads);
    }

    void TearDown() override
    {
        graph_destroy(graph);
    }
};


// Test that we error out properly when trying to add an edge with nodes that
// do not exist
TEST_F(GraphDlistFixture, AddEdgeTestFailure)
{
    int * invalid_node1 = get_payload(1);
    int * invalid_node2 = get_payload(2);
    uint32_t weight = 0;

    graph_opt_t result = graph_add_edge(this->graph, invalid_node1, invalid_node2, weight);
    EXPECT_EQ(result, GRAPH_FAIL_NODE_NODE_FOUND);

    free_payload(invalid_node1);
    free_payload(invalid_node2);
}

