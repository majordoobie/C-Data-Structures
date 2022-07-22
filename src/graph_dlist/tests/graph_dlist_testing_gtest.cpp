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
    graph_destroy(graph, NULL);
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
        graph_destroy(graph, free_payload);
    }
};


// Test that we error out properly when trying to add an edge with nodes that
// do not exist
TEST_F(GraphDlistFixture, AddEdgeTestFailure)
{
    node_t * node1 = graph_create_node(get_payload(1));
    graph_add_node(this->graph, node1);

    node_t * node2 = graph_create_node(get_payload(2));

    uint32_t weight = 0;

    graph_opt_t result = graph_add_edge(this->graph, node1, node2, weight);
    EXPECT_EQ(result, GRAPH_NODE_NOT_FOUND);

    graph_destroy_node(node2, free_payload);
}

TEST_F(GraphDlistFixture, )

