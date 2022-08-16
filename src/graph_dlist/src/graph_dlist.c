#include <graph_dlist.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

typedef struct graph_t
{
    dlist_t * nodes;
    graph_mode_t graph_mode;
    dlist_match_t (* compare_func)(void *, void *);
} graph_t;

typedef struct gnode_t
{
    void * data;
    dlist_t * edges;
} gnode_t;

typedef struct
{
    uint32_t weight;
    gnode_t * to_node;
} edge_t;

static edge_t * create_edge(gnode_t * to_node, uint32_t weight);
static dlist_match_t compare_nodes(void * left, void * right);
static void free_edge_dnode(void * node);
static void free_edges(dlist_t * edge);

/*!
 * @brief Initialize a adjacency list graph structure that uses a double linked
 * list for both the nodes and edges. When time allows, it will be best to
 * substitute the list of nodes for a hast table of nodes for significantly
 * increase lookup times.
 *
 * @param graph_mode GRAPH_DIRECTIONAL_TRUE OR GRAPH_DIRECTIONAL_FALSE
 * @param compare_func Function pointer for making comparisons between nodes
 * @return Pointer to graph object or NULL with a stdout message
 */
graph_t * graph_init(graph_mode_t graph_mode,
                     dlist_match_t (* compare_func)(void *, void *))
{
    graph_t * graph = (graph_t *)malloc(sizeof(graph_t));
    if (INVALID_PTR == verify_alloc(graph))
    {
        return NULL;
    }

    dlist_t * dlist = dlist_init(compare_func);
    if (NULL == dlist)
    {
        graph_destroy(graph, NULL);
        return NULL;
    }

    * graph = (graph_t){
        .nodes          = dlist,
        .compare_func   = compare_func,
        .graph_mode     = graph_mode
    };

    return graph;
}

static void get_hex_value(void * ptr, char * string, size_t buff_size)
{
    snprintf(string, buff_size, "%02hhX%02hhX",
             ((unsigned char *)&ptr)[1], ((unsigned char *)&ptr)[0]);

}

void graph_print(graph_t * graph)
{
    dlist_iter_t * nodes = dlist_get_iterable(graph->nodes, ITER_HEAD);
    gnode_t * node = iter_get_value(nodes);
    char buff[5];
    while (NULL != node)
    {
        get_hex_value(node, buff, sizeof(buff));
        printf("%s\n", buff);
        if (!graph_node_contain_edges(node))
        {
            printf("has edges");

        }
        node = dlist_get_iter_next(nodes);
    }
    dlist_destroy_iter(nodes);
}

/*!
 * @brief Wrapper for graph_add_node while also creating a node for the data
 * passed in.
 *
 * @param graph Pointer to the graph object
 * @param value Pointer to the value being stored in the graph
 * @return GRAPH_SUCCESS or GRAPH_FAIL_NODE_ALREADY_EXISTS
 */
graph_opt_t graph_add_value(graph_t * graph, void * value)
{
    gnode_t * node = graph_create_node(value);
    graph_opt_t result = graph_add_node(graph, node);

    // If the node already exists in the graph then free the node created
    // and return the error
    if (GRAPH_FAIL_NODE_ALREADY_EXISTS == result)
    {
        graph_destroy_node(node, NULL);
    }

    return result;
}

/*!
 * @brief Adds the provided node to the graph.
 *
 * @param graph Pointer to the graph object
 * @param node Pointer to the node object
 * @return GRAPH_SUCCESS or GRAPH_FAIL_NODE_ALREADY_EXISTS
 */
graph_opt_t graph_add_node(graph_t  * graph, gnode_t * node)
{
    assert(graph);
    assert(node);

    if (true == node_in_graph(graph, node))
    {
        return GRAPH_FAIL_NODE_ALREADY_EXISTS;
    }

    dlist_append(graph->nodes, node);
    return GRAPH_SUCCESS;
}

/*!
 * @brief Creates a node object with the provided node data
 * @param data Pointer to the data being stored in the graph
 * @return Pointer to the node object or NULL if invalid with a stderr message
 */
gnode_t * graph_create_node(void * data)
{
    gnode_t * node = (gnode_t *)malloc(sizeof(gnode_t));
    if (INVALID_PTR == verify_alloc(node))
    {
        return NULL;
    }

    dlist_t * dlist = dlist_init(compare_nodes);
    if (NULL == dlist)
    {
        graph_destroy_node(node, NULL);
    }

    * node = (gnode_t) {
        .data   = data,
        .edges  = dlist
    };

    return node;
}

gnode_t * graph_get_node_by_value(graph_t * graph, void * data)
{
    dlist_iter_t * nodes = dlist_get_iterable(graph->nodes, ITER_HEAD);
    gnode_t * node = iter_get_value(nodes);
    dlist_match_t result;

    while (NULL != node)
    {
        result = graph->compare_func(node->data, data);
        if (DLIST_MATCH == result)
        {
            break;
        }
        node = dlist_get_iter_next(nodes);
    }
    dlist_destroy_iter(nodes);
    return node;
}

bool node_in_graph(graph_t * graph, gnode_t * node)
{
    return value_in_graph(graph, node->data);
}

bool value_in_graph(graph_t * graph, void * data)
{
    gnode_t * graph_node = graph_get_node_by_value(graph, data);
    if (NULL == graph_node)
    {
        return false;
    }
    return true;
}

/*!
 * @brief Check weather the gnode has any edges
 * @param node Pointer to a gnode_t
 * @return Bool indication if there are any edges for the given gnode
 */
bool graph_node_contain_edges(gnode_t * node)
{
    return dlist_is_empty(node->edges);
}


/*!
 * @brief Destroy the given node object with the option to also free the data
 * stored in the graph using th function pointer provided. If the function
 * pointer is set to NULL then the function will not attempt to free the
 * data pointer.
 *
 * @param node Pointer to the node object being freed
 * @param free_func Function pointer to the free function to free the data
 * stored in the node. Set to NULL to prevent freeing of the data stored in
 * the node.
 */
void graph_destroy_node(gnode_t * node, void (*free_func)(void *))
{
    if (NULL != free_func)
    {
        free_func(node->data);
    }
    free_edges(node->edges);
    free(node);
}

/*!
 * @brief Add an edge between the two given nodes if they exist. A weight value
 * is used to indicate the weight between the two nodes. This option can be
 * omited with the NO_WEIGHT enum.
 *
 * @param graph Pointer to graph object
 * @param source_node Pointer to the gnode_t object
 * @param target_node Pointer to the gnode_t object
 * @param weight Value of the edge weight. Use NO_WEIGHT for default weight of 0
 * @return
 * GRAPH_SUCCESS If operation was successful.
 * GRAPH_EDGE_ALREADY_EXISTS if the edge already exists.
 * GRAPH_NODE_NOT_FOUND if node is the node attempting to link is not in the
 * graph
 */
graph_opt_t graph_add_edge(graph_t * graph,
                           gnode_t * source_node,
                           gnode_t * target_node,
                           uint32_t weight)
{
    assert(graph);
    assert(source_node);
    assert(target_node);

    // First make sure that both nodes are already in the graph
    if (!(node_in_graph(graph, source_node)) || !(node_in_graph(graph, target_node)))
    {
        return GRAPH_NODE_NOT_FOUND;
    }


    // First step is to see if the source node already has the edge for that
    // target node
    if (NULL == dlist_get_by_value(source_node->edges, target_node))
    {
        edge_t * edge = create_edge(target_node, weight);
        dlist_append(source_node->edges, edge);
    }
    else
    {
        return GRAPH_EDGE_ALREADY_EXISTS;
    }

    // finally, if the graph mode is unidirectional, then add the edge
    // going the opposite direction
    if (GRAPH_DIRECTIONAL_FALSE == graph->graph_mode)
    {
        // only add it if the edge does not exist
        if (NULL == dlist_get_by_value(target_node->edges, source_node))
        {
            edge_t * edge = create_edge(source_node, weight);
            dlist_append(target_node->edges, edge);
        }
    }

    return GRAPH_SUCCESS;
}

/*!
 * @brief Destroy the graph object with the option to free the data nodes in the
 * graph using function pointer that handles freeing the data nodes.
 *
 * @param graph Pointer to the graph object
 */
void graph_destroy(graph_t * graph, void (*free_func)(void *))
{
    // If for some reason nodes is null, just free the graph
    if (NULL != graph->nodes)
    {
        // If free function is provided then free the actual data stored in
        // the nodes before freeing the graph structure
        if (NULL != free_func)
        {
            dlist_iter_t * nodes = dlist_get_iterable(graph->nodes, ITER_HEAD);
            gnode_t * g_node = iter_get_value(nodes);
            while (NULL != g_node)
            {
                graph_destroy_node(g_node, free_func);
                g_node = dlist_get_iter_next(nodes);
            }
            dlist_destroy_iter(nodes);
        }
    }
    // free the graph structure
//    dlist_destroy_free(graph->nodes, NULL);
    dlist_destroy(graph->nodes);

    free(graph);
}

/*!
 * @brief Create an edge object
 * @param to_node Pointer to the gnode_t object that the edge leads to
 * @param weight Weight of the edge
 * @return Pointer to a edge object or NULL if invalid with a error message to
 * stderr.
 */
static edge_t * create_edge(gnode_t * to_node, uint32_t weight)
{
    edge_t * edge = (edge_t *)malloc(sizeof(edge_t));
    if (INVALID_PTR == verify_alloc(edge))
    {
        return NULL;
    }

    * edge = (edge_t){
        .to_node    = to_node,
        .weight     = weight
    };
    return edge;
}



static void free_edges(dlist_t * edge)
{
    dlist_destroy_free(edge, free_edge_dnode);
}

static void free_edge_dnode(void * node)
{
    free(node);
}

static dlist_match_t compare_nodes(void * left, void * right)
{
    if (left == right)
    {
        return DLIST_MATCH;
    }
    return DLIST_MISS_MATCH;
}


