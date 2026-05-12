#pragma once
#include <math.h>
#include <stdio.h>

typedef struct {
    int n;
    int m;
    int m_cap;
    int *inc;
    double *weights;
} Graph;

#ifdef __cplusplus
extern "C" {
#endif

int  graph_init(Graph *g, int n);
void graph_free(Graph *g);
int  graph_add_edge(Graph *g, int u, int v, double weight);
double *graph_build_adj_matrix(const Graph *g);
void graph_print_incidence_matrix(const Graph *g);

#ifdef __cplusplus
}
#endif
