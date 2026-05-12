#include "graph.h"
#include <stdlib.h>
#include <string.h>

int graph_init(Graph *g, int n) {
    if (n <= 0) return -1;
    g->n = n;
    g->m = 0;
    g->m_cap = 0;
    g->inc = NULL;
    g->weights = NULL;
    return 0;
}

void graph_free(Graph *g) {
    free(g->inc);
    free(g->weights);
    g->inc = NULL;
    g->weights = NULL;
}

static int grow(Graph *g) {
    int new_cap = g->m_cap == 0 ? 4 : g->m_cap * 2;

    int *new_inc = malloc(g->n * new_cap * sizeof(int));
    if (!new_inc) return -1;

    double *new_w = realloc(g->weights, new_cap * sizeof(double));
    if (!new_w) { free(new_inc); return -1; }

    for (int i = 0; i < g->n; ++i) {
        if (g->m_cap > 0)
            memcpy(&new_inc[i * new_cap], &g->inc[i * g->m_cap],
                   g->m_cap * sizeof(int));
        memset(&new_inc[i * new_cap + g->m_cap], 0,
               (new_cap - g->m_cap) * sizeof(int));
    }

    free(g->inc);
    g->inc = new_inc;
    g->weights = new_w;
    g->m_cap = new_cap;
    return 0;
}

int graph_add_edge(Graph *g, int u, int v, double weight) {
    if (u < 0 || u >= g->n || v < 0 || v >= g->n) return -1;
    if (u == v) return -2;
    if (g->m == g->m_cap && grow(g) != 0) return -3;

    g->inc[u * g->m_cap + g->m] =  1;
    g->inc[v * g->m_cap + g->m] = -1;
    g->weights[g->m] = weight;
    ++g->m;
    return 0;
}

double *graph_build_adj_matrix(const Graph *g) {
    int n = g->n;
    double *adj = malloc(n * n * sizeof(double));
    if (!adj) return NULL;

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            adj[i * n + j] = (i == j) ? 0.0 : INFINITY;

    for (int j = 0; j < g->m; ++j) {
        int src = -1, dst = -1;
        for (int i = 0; i < n; ++i) {
            int val = g->inc[i * g->m_cap + j];
            if      (val ==  1) src = i;
            else if (val == -1) dst = i;
        }
        if (src != -1 && dst != -1) {
            if (isinf(adj[src * n + dst]) || g->weights[j] < adj[src * n + dst])
                adj[src * n + dst] = g->weights[j];
        }
    }
    return adj;
}

void graph_print_incidence_matrix(const Graph *g) {
    printf("Incidence matrix (%d x %d):\n", g->n, g->m);
    if (g->m == 0) {
        printf("  (no edges)\n");
        return;
    }
    printf("    ");
    for (int j = 0; j < g->m; ++j) {
        char buf[16];
        snprintf(buf, sizeof(buf), "e%d", j);
        printf("%4s", buf);
    }
    printf("\n");
    for (int i = 0; i < g->n; ++i) {
        char buf[16];
        snprintf(buf, sizeof(buf), "v%d", i);
        printf("%-3s", buf);
        for (int j = 0; j < g->m; ++j)
            printf("%4d", g->inc[i * g->m_cap + j]);
        printf("\n");
    }
}
