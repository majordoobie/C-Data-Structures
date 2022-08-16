#ifndef DATA_STRUCTURES_C_SRC_GRAPH_DLIST_SRC_GRAPH_DLIST_H_
#define DATA_STRUCTURES_C_SRC_GRAPH_DLIST_SRC_GRAPH_DLIST_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <dl_list.h>

typedef enum
{
    GRAPH_SUCCESS,
    GRAPH_NODE_NOT_FOUND,
    GRAPH_FAIL_NODE_ALREADY_EXISTS,
    GRAPH_EDGE_ALREADY_EXISTS,
    GRAPH_EDGE_NOT_FOUND,
} graph_opt_t;

typedef enum
{
    GRAPH_DIRECTIONAL_TRUE,
    GRAPH_DIRECTIONAL_FALSE
} graph_mode_t;

typedef enum
{
    NO_WEIGHT = 0
} graph_defaults_t;


typedef struct graph_t graph_t;
typedef struct gnode_t gnode_t;

typedef struct
{
    uint32_t weight;
    gnode_t * from_node;
    gnode_t * to_node;
} edge_t;

graph_t * graph_init(graph_mode_t graph_mode,
                     dlist_match_t (* compare_func)(void *, void *));
void graph_destroy_node(gnode_t * node, void (*free_func)(void *));
void graph_destroy(graph_t * graph, void (* free_func)(void * data));

graph_opt_t graph_add_edge(graph_t * graph,
                           gnode_t * source_node,
                           gnode_t * target_node,
                           uint32_t weight);
graph_opt_t graph_remove_edge(graph_t * graph,
                              gnode_t * source_node,
                              gnode_t * target_node);

graph_opt_t graph_add_node(graph_t  * graph, gnode_t * node);
graph_opt_t graph_add_value(graph_t * graph, void * value);

// nodes
gnode_t * graph_create_node(void * data);
gnode_t * graph_get_node_by_value(graph_t * graph, void * data);
void graph_print(graph_t * graph);

// Queries
bool value_in_graph(graph_t * graph, void * data);
bool node_in_graph(graph_t * graph, gnode_t * node);
bool graph_node_contain_edges(gnode_t * node);
size_t graph_edge_count(gnode_t * node);
edge_t * graph_get_edge(graph_t * graph, gnode_t * source_node,
                        gnode_t * target_node);
bool graph_edge_in_graph(graph_t * graph, edge_t * edge);
#ifdef __cplusplus
}
#endif // end __cplusplus
#endif //DATA_STRUCTURES_C_SRC_GRAPH_DLIST_SRC_GRAPH_DLIST_H_
