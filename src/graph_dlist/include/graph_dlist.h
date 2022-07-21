#ifndef DATA_STRUCTURES_C_SRC_GRAPH_DLIST_SRC_GRAPH_DLIST_H_
#define DATA_STRUCTURES_C_SRC_GRAPH_DLIST_SRC_GRAPH_DLIST_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <dl_list.h>

typedef struct graph_t graph_t;
typedef enum
{
    GRAPH_SUCCESS,
    GRAPH_FAIL_NODE_NODE_FOUND,
    GRAPH_EDGE_ALREADY_EXISTS
} graph_opt_t;

typedef enum
{
    GRAPH_DIRECTIONAL_TRUE,
    GRAPH_DIRECTIONAL_FALSE
} graph_mode_t;

graph_t * graph_init(graph_mode_t graph_mode,
                     dlist_match_t (* compare_func)(void *, void *));
void graph_destroy(graph_t * graph);

graph_opt_t graph_add_edge(graph_t * graph,
                           void * source_node_p,
                           void * target_node_p,
                           uint32_t weight);

#ifdef __cplusplus
}
#endif // end __cplusplus
#endif //DATA_STRUCTURES_C_SRC_GRAPH_DLIST_SRC_GRAPH_DLIST_H_
