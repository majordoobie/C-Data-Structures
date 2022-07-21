#include <graph_dlist.h>
#include <stdlib.h>

typedef struct graph_t
{
    dlist_t * nodes;
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

graph_t * graph_init(dlist_match_t (* compare_func)(void *, void *))
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

    graph->nodes = dlist;
    return graph;
}

void graph_destroy(graph_t * graph)
{
    if (NULL != graph->nodes)
    {
        dlist_destroy(graph->nodes);
    }
    free(graph);
}

