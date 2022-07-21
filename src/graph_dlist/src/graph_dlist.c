#include <graph_dlist.h>
#include <stdlib.h>
#include <assert.h>

typedef struct graph_t
{
    dlist_t * nodes;
    graph_mode_t graph_mode;
    dlist_match_t (* compare_func)(void *, void *);
} graph_t;

typedef struct
{
    void * data;
    dlist_t * edges;
} node_t;

typedef struct
{
    uint32_t weight;
    node_t * to_node;
} edge_t;

static edge_t * get_edge(node_t * to_node, uint32_t weight);

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
        graph_destroy(graph);
        return NULL;
    }

    *graph = (graph_t){
        .nodes          = dlist,
        .compare_func   = compare_func,
        .graph_mode     = graph_mode
    };

    return graph;
}



graph_opt_t graph_add_edge(graph_t * graph,
                           void * source_node_p,
                           void * target_node_p,
                           uint32_t weight)
{
    assert(graph);
    assert(source_node_p);
    assert(target_node_p);

    // first step is finding if the source_node exists
    node_t * source_node = dlist_get_by_value(graph->nodes, source_node_p);
    if (NULL == source_node)
    {
        return GRAPH_FAIL_NODE_NODE_FOUND;
    }

    // next find if the target_node exists
    node_t * target_node = dlist_get_by_value(graph->nodes, target_node_p);
    if (NULL == target_node)
    {
        return GRAPH_FAIL_NODE_NODE_FOUND;
    }

    // next step is to add the edge between the source and target node if
    // the edge does not already exist
    if (NULL == dlist_get_by_value(source_node->edges, target_node))
    {
        edge_t * edge = get_edge(target_node, weight);
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
            edge_t * edge = get_edge(source_node, weight);
            dlist_append(target_node->edges, edge);
        }
    }

    return GRAPH_SUCCESS;
}

void graph_destroy(graph_t * graph)
{
    if (NULL != graph->nodes)
    {
        dlist_destroy(graph->nodes);
    }
    free(graph);
}

static edge_t * get_edge(node_t * to_node, uint32_t weight)
{
    edge_t * edge = (edge_t*)malloc(sizeof(edge_t));
    if (INVALID_PTR == verify_alloc(edge))
    {
        return NULL;
    }

    *edge = (edge_t) {
        .to_node    = to_node,
        .weight     = weight
    };
    return edge;
}











