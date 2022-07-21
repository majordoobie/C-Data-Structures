#ifndef DATA_STRUCTURES_C_SRC_GRAPH_DLIST_SRC_GRAPH_DLIST_H_
#define DATA_STRUCTURES_C_SRC_GRAPH_DLIST_SRC_GRAPH_DLIST_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <dl_list.h>

typedef struct graph_t graph_t;

graph_t * graph_init(dlist_match_t (* compare_func)(void *, void *));
void graph_destroy(graph_t * graph);

#ifdef __cplusplus
}
#endif // end __cplusplus
#endif //DATA_STRUCTURES_C_SRC_GRAPH_DLIST_SRC_GRAPH_DLIST_H_
