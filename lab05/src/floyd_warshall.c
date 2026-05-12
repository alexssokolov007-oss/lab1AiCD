#include "floyd_warshall.h"
#include <stdlib.h>
#include <math.h>

FWResult floyd_warshall(const Graph *g) {
    int n = g->n;
    FWResult res;
    res.n = n;
    res.has_negative_cycle = 0;
    res.dist = graph_build_adj_matrix(g);
    res.next = malloc(n * n * sizeof(int));

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            res.next[i * n + j] = (i != j && !isinf(res.dist[i * n + j])) ? j : -1;

    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            if (isinf(res.dist[i * n + k])) continue;
            for (int j = 0; j < n; ++j) {
                if (isinf(res.dist[k * n + j])) continue;
                double via = res.dist[i * n + k] + res.dist[k * n + j];
                if (via < res.dist[i * n + j]) {
                    res.dist[i * n + j] = via;
                    res.next[i * n + j] = res.next[i * n + k];
                }
            }
        }
    }

    for (int i = 0; i < n; ++i)
        if (res.dist[i * n + i] < 0.0) { res.has_negative_cycle = 1; break; }

    return res;
}

void fw_result_free(FWResult *r) {
    free(r->dist);
    free(r->next);
    r->dist = NULL;
    r->next = NULL;
}

int *reconstruct_path(const FWResult *res, int u, int v, int *path_len) {
    if (res->next[u * res->n + v] == -1) {
        *path_len = 0;
        return NULL;
    }

    int *path = malloc(res->n * sizeof(int));
    *path_len = 0;

    int cur = u;
    path[(*path_len)++] = cur;
    while (cur != v) {
        cur = res->next[cur * res->n + v];
        path[(*path_len)++] = cur;
    }
    return path;
}
