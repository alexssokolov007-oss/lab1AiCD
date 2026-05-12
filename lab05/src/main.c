#include <stdio.h>
#include <math.h>
#include "graph.h"
#include "floyd_warshall.h"

static void print_dist_matrix(const FWResult *res, int n) {
    printf("\nMatrix of shortest distances:\n");
    printf("%6s", "");
    for (int j = 0; j < n; ++j) {
        char buf[16];
        snprintf(buf, sizeof(buf), "v%d", j);
        printf("%9s", buf);
    }
    printf("\n");
    for (int i = 0; i < n; ++i) {
        char buf[16];
        snprintf(buf, sizeof(buf), "v%d", i);
        printf("%-4s", buf);
        for (int j = 0; j < n; ++j) {
            double d = res->dist[i * n + j];
            if (isinf(d))
                printf("%9s", "INF");
            else
                printf("%9.2f", d);
        }
        printf("\n");
    }
}

static void print_paths(const FWResult *res, int n) {
    printf("\nShortest paths:\n");
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;
            printf("v%d -> v%d:  ", i, j);
            if (isinf(res->dist[i * n + j])) {
                printf("no path\n");
            } else {
                int path_len;
                int *path = reconstruct_path(res, i, j, &path_len);
                for (int k = 0; k < path_len; ++k) {
                    if (k) printf(" -> ");
                    printf("v%d", path[k]);
                }
                printf("  (dist = %.2f)\n", res->dist[i * n + j]);
                free(path);
            }
        }
    }
}

int main(void) {
    printf("Lab 05: Incidence Matrix + Floyd-Warshall\n\n");

    int n = 0;
    printf("Enter number of vertices: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        fprintf(stderr, "Invalid vertex count.\n");
        return 1;
    }

    Graph g;
    graph_init(&g, n);

    int m = 0;
    printf("Enter number of edges: ");
    if (scanf("%d", &m) != 1 || m < 0) {
        fprintf(stderr, "Invalid edge count.\n");
        graph_free(&g);
        return 1;
    }

    for (int i = 0; i < m; ++i) {
        int u, v;
        double w;
        printf("Edge %d (from to weight): ", i);
        if (scanf("%d %d %lf", &u, &v, &w) != 3) {
            fprintf(stderr, "Invalid input.\n");
            graph_free(&g);
            return 1;
        }
        int rc = graph_add_edge(&g, u, v, w);
        if (rc == -1) {
            fprintf(stderr, "Vertex index out of range.\n");
            --i;
        } else if (rc == -2) {
            fprintf(stderr, "Self-loops are not supported.\n");
            --i;
        } else if (rc == -3) {
            fprintf(stderr, "Memory allocation failed.\n");
            graph_free(&g);
            return 1;
        }
    }

    printf("\n");
    graph_print_incidence_matrix(&g);

    FWResult res = floyd_warshall(&g);

    if (res.has_negative_cycle) {
        printf("\nGraph contains a negative-weight cycle.\n");
    } else {
        print_dist_matrix(&res, n);
        print_paths(&res, n);
    }

    fw_result_free(&res);
    graph_free(&g);
    return 0;
}
