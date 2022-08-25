#include <gtest/gtest.h>
#include <graph_dlist.h>
#include <hashtable.h>

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

uint64_t hash_callback(void * payload)
{
    int * value = (int *)graph_get_node_value((gnode_t * )payload);
    uint64_t hash = htable_get_init_hash();
    htable_hash_key(&hash, value, sizeof(int));
    return hash;
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

// print callback
char * print_callback(void * node)
{
    int value = *(int*)node;
    int length = snprintf(NULL, 0, "%d", value);
    length++;
    char * str = (char*) malloc((size_t)length);
    snprintf( str, (size_t)length, "%d", value);
    return str;
}


TEST(GraphBasic, TestBasicStartUp)
{
    graph_t * graph = graph_init(GRAPH_DIRECTED, compare_payloads, hash_callback);
    EXPECT_NE(graph, nullptr);
    graph_destroy(graph, nullptr);
}

class GraphDlistFixture : public ::testing::Test
{
 public:
    graph_t * graph{};
    std::vector<int>graph_data = {0, 1, 2, 3, 4, 5, 6, 7, 8};

    int safe_test_value1 = 100;

 protected:
    void SetUp() override
    {
        graph = graph_init(GRAPH_DIRECTED, compare_payloads, hash_callback);
        for (auto& value: graph_data)
        {
            graph_add_value(graph, get_payload(value));
        }
        /*
         * (0)--2--(1)--5--(4)--2--(5)--2--(6)
         *  |	    |       \       |       |
         *  3       1       1       1       1
         *  |       |        \      |       |
         * (3)--2--(2)        \____(8)--1--(7)
         */

        gnode_t * node0 = graph_get_node_by_value(this->graph, &this->graph_data.at(0));
        gnode_t * node1 = graph_get_node_by_value(this->graph, &this->graph_data.at(1));
        gnode_t * node2 = graph_get_node_by_value(this->graph, &this->graph_data.at(2));
        gnode_t * node3 = graph_get_node_by_value(this->graph, &this->graph_data.at(3));
        gnode_t * node4 = graph_get_node_by_value(this->graph, &this->graph_data.at(4));
        gnode_t * node5 = graph_get_node_by_value(this->graph, &this->graph_data.at(5));
        gnode_t * node6 = graph_get_node_by_value(this->graph, &this->graph_data.at(6));
        gnode_t * node7 = graph_get_node_by_value(this->graph, &this->graph_data.at(7));
        gnode_t * node8 = graph_get_node_by_value(this->graph, &this->graph_data.at(8));

        // 0 -> 1/3
        graph_add_edge(graph, node0, node1, 2);
        graph_add_edge(graph, node0, node3, 3);

        // 3 -> 2
        graph_add_edge(graph, node3, node2, 2);

        //2 -> 1
        graph_add_edge(graph, node2, node1, 1);

        // 1 -> 4
        graph_add_edge(graph, node1, node4, 5);

        // 4 -> 5/8
        graph_add_edge(graph, node4, node5, 2);
        graph_add_edge(graph, node4, node8, 1);

        // 8 -> 7/5
        graph_add_edge(graph, node8, node7, 1);
        graph_add_edge(graph, node8, node5, 1);

        // 7 -> 6
        graph_add_edge(graph, node7, node6, 1);

        // 6 -> 5
        graph_add_edge(graph, node6, node5, 2);


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
    gnode_t * node0 = graph_get_node_by_value(this->graph, &this->graph_data.at(0));
    EXPECT_EQ(2, graph_edge_count(node0));

    gnode_t * node6 = graph_get_node_by_value(this->graph, &this->graph_data.at(6));
    EXPECT_EQ(2, graph_edge_count(node6));


    uint32_t weight = 0;
    EXPECT_EQ(graph_add_edge(this->graph, node0, node6, weight), GRAPH_SUCCESS);
    EXPECT_EQ(3, graph_edge_count(node0));
}

//Test ability to remove an edge that exist and an edge that does not
TEST_F(GraphDlistFixture, RemoveEdgeTest)
{
    gnode_t * node0 = graph_get_node_by_value(this->graph, &this->graph_data.at(0));
    gnode_t * node1 = graph_get_node_by_value(this->graph, &this->graph_data.at(1));

    EXPECT_EQ(2, graph_edge_count(node0));
    EXPECT_EQ(3, graph_edge_count(node1));

    EXPECT_EQ(graph_remove_edge(this->graph, node0, node1), GRAPH_SUCCESS);

    EXPECT_EQ(1, graph_edge_count(node0));
    EXPECT_EQ(2, graph_edge_count(node1));

    // Test removal of an edge that does not exist
    EXPECT_EQ(graph_remove_edge(this->graph, node0, node1), GRAPH_EDGE_NOT_FOUND);
}

// Test ability to fetch an edge from the graph
TEST_F(GraphDlistFixture, FindEdgeTest)
{
    gnode_t * node0 = graph_get_node_by_value(this->graph, &this->graph_data.at(0));
    gnode_t * node1 = graph_get_node_by_value(this->graph, &this->graph_data.at(1));

    edge_t * edge = graph_get_edge(this->graph, node0, node1);
    EXPECT_EQ(edge->from_node, node0);
    EXPECT_EQ(edge->to_node, node1);
}


// Test ability to test if an edge is in the graph
TEST_F(GraphDlistFixture, EdgeInGraph)
{
    gnode_t * node0 = graph_get_node_by_value(this->graph, &this->graph_data.at(0));
    gnode_t * node1 = graph_get_node_by_value(this->graph, &this->graph_data.at(1));

    edge_t * edge = graph_get_edge(this->graph, node0, node1);
    EXPECT_EQ(true, graph_edge_in_graph(this->graph, edge));
}

// Test ability to get all edges of a node and also see if a node has an edge
// to another node
TEST_F(GraphDlistFixture, EdgeInNode)
{
    gnode_t * node0 = graph_get_node_by_value(this->graph, &this->graph_data.at(0));
    gnode_t * node1 = graph_get_node_by_value(this->graph, &this->graph_data.at(1));
    gnode_t * node6 = graph_get_node_by_value(this->graph, &this->graph_data.at(6));

    EXPECT_EQ(true, graph_node_a_neighbor(node0, node1));
    EXPECT_EQ(false, graph_node_a_neighbor(node0, node6));
}

// Test ability to iterate over the neighbors of a node
TEST_F(GraphDlistFixture, IterateNeighbors)
{
    gnode_t * node0 = graph_get_node_by_value(this->graph, &this->graph_data.at(0));
    dlist_iter_t * neighbors = graph_get_neighbors_list(node0);
    edge_t * neighbor = (edge_t *)(iter_get_value(neighbors));
    int value = 1;
    while (NULL != neighbor)
    {
        EXPECT_EQ(value, *(int*)graph_get_node_value(neighbor->to_node));
        neighbor = (edge_t *)dlist_get_iter_next(neighbors);
        value = value + 2; // The next neighbor is 3
    }

    graph_destroy_neighbors_list(neighbors);
}

// Test ability to remove a node and all its edges
TEST_F(GraphDlistFixture, TestNodeRemoval)
{
    EXPECT_EQ(this->graph_data.size(), graph_node_count(this->graph));
    gnode_t * node4 = graph_get_node_by_value(this->graph, &this->graph_data.at(4));
    gnode_t * node1 = graph_get_node_by_value(this->graph, &this->graph_data.at(1));
    gnode_t * node5 = graph_get_node_by_value(this->graph, &this->graph_data.at(5));
    gnode_t * node8 = graph_get_node_by_value(this->graph, &this->graph_data.at(8));

    EXPECT_EQ(3, graph_edge_count(node4));
    EXPECT_EQ(3, graph_edge_count(node1));
    EXPECT_EQ(3, graph_edge_count(node5));
    EXPECT_EQ(3, graph_edge_count(node8));

    graph_remove_node(this->graph, node4, free_payload);

    EXPECT_EQ(this->graph_data.size() - 1, graph_node_count(this->graph));
    EXPECT_EQ(2, graph_edge_count(node1));
    EXPECT_EQ(2, graph_edge_count(node5));
    EXPECT_EQ(2, graph_edge_count(node8));
}


// Test ability to find the path from 1 to 4
/*
 * (0)--2--(1)--5--(4)--2--(5)--2--(6)
 *  |	    |       \       |       |
 *  3       1       1       1       1
 *  |       |        \      |       |
 * (3)--2--(2)        \____(8)--1--(7)
 */
TEST_F(GraphDlistFixture, TestPathFinding2)
{
    gnode_t * node4 = graph_get_node_by_value(this->graph, &this->graph_data.at(4));
    gnode_t * node1 = graph_get_node_by_value(this->graph, &this->graph_data.at(1));

    std::vector<int> expected_path = {1, 4};

    path_t * path = graph_get_path(this->graph, node1, node4);
    EXPECT_EQ(path->path_weight, 5);

    dlist_iter_t * iter = dlist_get_iterable(path->path, ITER_HEAD);
    gnode_t * node = (gnode_t *)iter_get_value(iter);
    int count = 0;
    while (NULL != node)
    {
        int data = *(int*)graph_get_node_value(node);
        EXPECT_EQ(data, expected_path.at((unsigned long)count));
        node = (gnode_t *)dlist_get_iter_next(iter);
        count++;
    }

    dlist_destroy_iter(iter);
    graph_free_path(path);
}


// Test ability to find the path from 0 to 7
/*
 * (0)--2--(1)--5--(4)--2--(5)--2--(6)
 *  |	    |       \       |       |
 *  3       1       1       1       1
 *  |       |        \      |       |
 * (3)--2--(2)        \____(8)--1--(7)
 */
TEST_F(GraphDlistFixture, TestPathFinding)
{
    gnode_t * node0 = graph_get_node_by_value(this->graph, &this->graph_data.at(0));
    gnode_t * node7 = graph_get_node_by_value(this->graph, &this->graph_data.at(7));

    std::vector<int> expected_path = {0, 1, 4, 8, 7};
    size_t expected_weight = 9;

    path_t * path = graph_get_path(this->graph, node0, node7);
    EXPECT_EQ(path->path_weight, expected_weight);

    dlist_iter_t * iter = dlist_get_iterable(path->path, ITER_HEAD);
    gnode_t * node = (gnode_t *)iter_get_value(iter);
    int count = 0;
    while (NULL != node)
    {
        int data = *(int*)graph_get_node_value(node);
        EXPECT_EQ(data, expected_path.at((unsigned long)count));
        node = (gnode_t *)dlist_get_iter_next(iter);
        count++;
    }

    dlist_destroy_iter(iter);
    graph_free_path(path);
}

// Test ability to find the path from 3 to 8
/*
 * (0)--2--(1)--5--(4)--2--(5)--2--(6)
 *  |	    |       \       |       |
 *  3       1       1       1       1
 *  |       |        \      |       |
 * (3)--2--(2)        \____(8)--1--(7)
 */
TEST_F(GraphDlistFixture, TestPathFinding3)
{
    gnode_t * node3 = graph_get_node_by_value(this->graph, &this->graph_data.at(3));
    gnode_t * node8 = graph_get_node_by_value(this->graph, &this->graph_data.at(8));

    std::vector<int> expected_path = {3, 2, 1, 4, 8};
    size_t expected_weight = 9;

    path_t * path = graph_get_path(this->graph, node3, node8);
    EXPECT_EQ(path->path_weight, expected_weight);

    dlist_iter_t * iter = dlist_get_iterable(path->path, ITER_HEAD);
    gnode_t * node = (gnode_t *)iter_get_value(iter);
    int count = 0;
    while (NULL != node)
    {
        int data = *(int*)graph_get_node_value(node);
        EXPECT_EQ(data, expected_path.at((unsigned long)count));
        node = (gnode_t *)dlist_get_iter_next(iter);
        count++;
    }

    dlist_destroy_iter(iter);
    graph_free_path(path);
}

// Test ability to find the path from 6 to 6
/*
 * (0)--2--(1)--5--(4)--2--(5)--2--(6)
 *  |	    |       \       |       |
 *  3       1       1       1       1
 *  |       |        \      |       |
 * (3)--2--(2)        \____(8)--1--(7)
 */
TEST_F(GraphDlistFixture, TestPathFinding4)
{
    gnode_t * node6 = graph_get_node_by_value(this->graph, &this->graph_data.at(6));

    std::vector<int> expected_path = {6};
    size_t expected_weight = 0;

    path_t * path = graph_get_path(this->graph, node6, node6);
    EXPECT_EQ(path->path_weight, expected_weight);

    dlist_iter_t * iter = dlist_get_iterable(path->path, ITER_HEAD);
    gnode_t * node = (gnode_t *)iter_get_value(iter);
    int count = 0;
    while (NULL != node)
    {
        int data = *(int*)graph_get_node_value(node);
        EXPECT_EQ(data, expected_path.at((unsigned long)count));
        node = (gnode_t *)dlist_get_iter_next(iter);
        count++;
    }

    dlist_destroy_iter(iter);
    graph_free_path(path);
}

// Test ability to find the path from 6 to 4
/*
 * (0)--2--(1)--5--(4)--2--(5)--2--(6)
 *  |	    |       \       |       |
 *  3       1       1       1       1
 *  |       |        \      |       |
 * (3)--2--(2)        \____(8)--1--(7)
 */
TEST_F(GraphDlistFixture, TestPathFinding5)
{
    gnode_t * node6 = graph_get_node_by_value(this->graph, &this->graph_data.at(6));
    gnode_t * node4 = graph_get_node_by_value(this->graph, &this->graph_data.at(4));

    std::vector<int> expected_path = {6, 7, 8, 4};
    size_t expected_weight = 3;

    path_t * path = graph_get_path(this->graph, node6, node4);
    EXPECT_EQ(path->path_weight, expected_weight);

    dlist_iter_t * iter = dlist_get_iterable(path->path, ITER_HEAD);
    gnode_t * node = (gnode_t *)iter_get_value(iter);
    int count = 0;
    while (NULL != node)
    {
        int data = *(int*)graph_get_node_value(node);
        EXPECT_EQ(data, expected_path.at((unsigned long)count));
        node = (gnode_t *)dlist_get_iter_next(iter);
        count++;
    }

    dlist_destroy_iter(iter);
    graph_free_path(path);
}

// Test ability to find the path from 6 to 4
/*
 * (0)--2--(1)--5--(4)--2--(5)--2--(6)
 *  |	    |       \       |       |
 *  3       1       1       1       1
 *  |       |        \      |       |
 * (3)--2--(2)        \____(8)--1--(7)
 */
TEST_F(GraphDlistFixture, TestPathFinding6)
{
    gnode_t * node6 = graph_get_node_by_value(this->graph, &this->graph_data.at(6));
    gnode_t * node4 = graph_get_node_by_value(this->graph, &this->graph_data.at(4));

    std::vector<int> expected_path = {6, 7, 8, 4};
    size_t expected_weight = 3;

    path_t * path = graph_get_path(this->graph, node6, node4);
    EXPECT_EQ(path->path_weight, expected_weight);

    dlist_iter_t * iter = dlist_get_iterable(path->path, ITER_HEAD);
    gnode_t * node = (gnode_t *)iter_get_value(iter);
    int count = 0;
    while (NULL != node)
    {
        int data = *(int*)graph_get_node_value(node);
        EXPECT_EQ(data, expected_path.at((unsigned long)count));
        node = (gnode_t *)dlist_get_iter_next(iter);
        count++;
    }

    dlist_destroy_iter(iter);
    graph_free_path(path);
}

// Test ability to find the path from 3 to 6
/*
 * (0)--2--(1)--5--(4)--2--(5)--2--(6)
 *  |	    |       \       |       |
 *  3       1       1       1       1
 *  |       |        \      |       |
 * (3)--2--(2)        \____(8)--1--(7)
 */
TEST_F(GraphDlistFixture, TestPathFinding7)
{
    gnode_t * node3 = graph_get_node_by_value(this->graph, &this->graph_data.at(3));
    gnode_t * node6 = graph_get_node_by_value(this->graph, &this->graph_data.at(6));

    std::vector<int> expected_path = {3, 2, 1, 4, 8, 7, 6};
    size_t expected_weight = 11;

    path_t * path = graph_get_path(this->graph, node3, node6);
    EXPECT_EQ(path->path_weight, expected_weight);

    dlist_iter_t * iter = dlist_get_iterable(path->path, ITER_HEAD);
    gnode_t * node = (gnode_t *)iter_get_value(iter);
    int count = 0;
    while (NULL != node)
    {
        int data = *(int*)graph_get_node_value(node);
        EXPECT_EQ(data, expected_path.at((unsigned long)count));
        node = (gnode_t *)dlist_get_iter_next(iter);
        count++;
    }

    dlist_destroy_iter(iter);
    graph_free_path(path);
}
