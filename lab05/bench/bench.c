#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "graph.h"
#include "floyd_warshall.h"

static double measure(int n, int edge_count, unsigned seed) {
    srand(seed);

    Graph g;
    graph_init(&g, n);

    int added = 0;
    for (int attempt = 0; attempt < edge_count * 4 && added < edge_count; ++attempt) {
        int u = rand() % n;
        int v = rand() % n;
        if (u == v) continue;
        double w = 1.0 + (rand() % 9901) / 100.0;
        if (graph_add_edge(&g, u, v, w) == 0)
            ++added;
    }

    clock_t t0 = clock();
    FWResult res = floyd_warshall(&g);
    clock_t t1 = clock();

    fw_result_free(&res);
    graph_free(&g);

    return (double)(t1 - t0) * 1000.0 / CLOCKS_PER_SEC;
}

int main(void) {
    printf("Floyd-Warshall Benchmark\n\n");

    static const struct { int n; int edges; } cases[] = {
        {   10,    20 },
        {   50,   200 },
        {  100,   500 },
        {  200,  1500 },
        {  500,  5000 },
        { 1000, 15000 },
    };
    const int ncases = (int)(sizeof(cases) / sizeof(cases[0]));
    const int RUNS = 5;

    printf("%-10s%-10s%-16s\n", "Vertices", "Edges", "Avg time (ms)");

    for (int i = 0; i < ncases; ++i) {
        double total = 0.0;
        for (int r = 0; r < RUNS; ++r)
            total += measure(cases[i].n, cases[i].edges, 42u + (unsigned)r);
        printf("%-10d%-10d%-16.3f\n", cases[i].n, cases[i].edges, total / RUNS);
    }

    return 0;
}
