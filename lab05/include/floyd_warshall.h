#pragma once
#include "graph.h"

typedef struct {
    double *dist;
    int *next;
    int n;
    int has_negative_cycle;
} FWResult;

#ifdef __cplusplus
extern "C" {
#endif

FWResult floyd_warshall(const Graph *g);
void fw_result_free(FWResult *r);
int *reconstruct_path(const FWResult *res, int u, int v, int *path_len);

#ifdef __cplusplus
}
#endif
