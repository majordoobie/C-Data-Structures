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
    if (*(int*)data1 == *(int*)data2)
    {
        return DLIST_MATCH;
    }
    return DLIST_MISS_MATCH;
}


TEST(GraphBasic, TestBasicStartUp)
{
    graph_t * graph = graph_init(GRAPH_DIRECTIONAL_FALSE, compare_payloads);
    EXPECT_NE(graph, nullptr);
    graph_destroy(graph, nullptr);
}

class GraphDlistFixture : public ::testing::Test
{
 public:
    graph_t * graph{};
    std::vector<int>graph_data = {1, 2, 3, 4, 5, 6, 7};

    int safe_test_value1 = 100;

 protected:
    void SetUp() override
    {
        graph = graph_init(GRAPH_DIRECTIONAL_TRUE, compare_payloads);
        for (auto& value: graph_data)
        {
            graph_add_value(graph, get_payload(value));
        }
    }

    void TearDown() override
    {
        graph_destroy(graph, free_payload);
    }
};

// Ensure that when adding a node that already exists that it errors out
TEST_F(GraphDlistFixture, AddNodeFailure)
{
    int * value_to_add = get_payload(this->graph_data.at(0));
    graph_opt_t res = graph_add_value(this->graph, value_to_add);

    EXPECT_EQ(res, GRAPH_FAIL_NODE_ALREADY_EXISTS);

    free_payload(value_to_add);
}

// Test that we error out properly when trying to add an edge with nodes that
// does not exist
TEST_F(GraphDlistFixture, AddEdgeTestFailure)
{
    gnode_t * node1 = graph_get_node_by_value(this->graph, &this->graph_data.at(0));

    gnode_t * node2 = graph_create_node(get_payload(this->safe_test_value1));
    uint32_t weight = 0;

    graph_opt_t result = graph_add_edge(this->graph, node1, node2, weight);
    EXPECT_EQ(result, GRAPH_NODE_NOT_FOUND);

    graph_destroy_node(node2, free_payload);
}

// Test ability to successfully add edges
TEST_F(GraphDlistFixture, AddEdgeTest)
{
    gnode_t * node1 = graph_get_node_by_value(this->graph, &this->graph_data.at(0));
    gnode_t * node2 = graph_get_node_by_value(this->graph, &this->graph_data.at(1));
    gnode_t * node3 = graph_get_node_by_value(this->graph, &this->graph_data.at(2));
    uint32_t weight = 0;

    EXPECT_EQ(graph_add_edge(this->graph, node1, node2, weight), GRAPH_SUCCESS);
    EXPECT_EQ(1, graph_edge_count(node1));

    EXPECT_EQ(graph_add_edge(this->graph, node2, node1, weight), GRAPH_SUCCESS);
    EXPECT_EQ(1, graph_edge_count(node2));

    EXPECT_EQ(graph_add_edge(this->graph, node1, node3, weight), GRAPH_SUCCESS);
    EXPECT_EQ(2, graph_edge_count(node1));
    EXPECT_EQ(graph_add_edge(this->graph, node3, node2, weight), GRAPH_SUCCESS);
}

//Test ability to remove an edge that exist and an edge that does not
TEST_F(GraphDlistFixture, RemoveEdgeTest)
{
    gnode_t * node1 = graph_get_node_by_value(this->graph, &this->graph_data.at(0));
    gnode_t * node2 = graph_get_node_by_value(this->graph, &this->graph_data.at(1));
    uint32_t weight = 0;

    EXPECT_EQ(graph_add_edge(this->graph, node1, node2, weight), GRAPH_SUCCESS);
    EXPECT_EQ(1, graph_edge_count(node1));

    EXPECT_EQ(graph_add_edge(this->graph, node2, node1, weight), GRAPH_SUCCESS);
    EXPECT_EQ(1, graph_edge_count(node2));

    EXPECT_EQ(graph_remove_edge(this->graph, node1, node2), GRAPH_SUCCESS);
    EXPECT_EQ(0, graph_edge_count(node1));

    // Test removal of an edge that does not exist
    EXPECT_EQ(graph_remove_edge(this->graph, node1, node2), GRAPH_EDGE_NOT_FOUND);
}

//Test ability to find an edge that exists and one that does not
TEST_F(GraphDlistFixture, FindEdgeTest)
{
    gnode_t * node1 = graph_get_node_by_value(this->graph, &this->graph_data.at(0));
    gnode_t * node2 = graph_get_node_by_value(this->graph, &this->graph_data.at(1));
    uint32_t weight = 0;

    EXPECT_EQ(graph_add_edge(this->graph, node1, node2, weight), GRAPH_SUCCESS);
    EXPECT_EQ(1, graph_edge_count(node1));

    EXPECT_EQ(graph_add_edge(this->graph, node2, node1, weight), GRAPH_SUCCESS);
    EXPECT_EQ(1, graph_edge_count(node2));

    edge_t * edge = graph_get_edge(this->graph, node1, node2);
    EXPECT_EQ(edge->from_node, node1);
    EXPECT_EQ(edge->to_node, node2);
}