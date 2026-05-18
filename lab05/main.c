#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#ifdef _WIN32
#include <windows.h>
#endif

/* Лаб. работа 5, Вариант 19
   Представление графа: матрица инцидентности
   Алгоритм: Флойда-Уоршела */

#define INF (LLONG_MAX / 4)

/*
 * Матрица инцидентности хранится как одномерный массив inc[nv * ne].
 * inc[v * ne + e] = +1  — вершина v является истоком ребра e
 * inc[v * ne + e] = -1  — вершина v является стоком ребра e
 * inc[v * ne + e] =  0  — вершина v не инцидентна ребру e
 * Веса рёбер хранятся отдельно в массиве w[ne].
 */
typedef struct {
    int       *inc;
    long long *w;
    int        nv;
    int        ne;
} Graph;

static Graph *graph_new(int nv, int ne) {
    Graph *g = (Graph *)malloc(sizeof(Graph));
    g->inc = (int *)calloc(nv * ne, sizeof(int));
    g->w   = (long long *)malloc(ne * sizeof(long long));
    g->nv  = nv;
    g->ne  = ne;
    return g;
}

static void graph_free(Graph *g) {
    free(g->inc);
    free(g->w);
    free(g);
}

/*
 * Формат входного файла:
 *   n m          <- число вершин и рёбер
 *   u v w        <- ребро из u в v, вес w (вершины с 0)
 *   ...
 */
static Graph *graph_read(FILE *f) {
    int nv, ne;
    if (fscanf(f, "%d %d", &nv, &ne) != 2 || nv < 1 || ne < 0)
        return NULL;

    Graph *g = graph_new(nv, ne);
    for (int e = 0; e < ne; e++) {
        int u, v;
        long long w;
        if (fscanf(f, "%d %d %lld", &u, &v, &w) != 3) {
            graph_free(g);
            return NULL;
        }
        g->inc[u * ne + e] =  1;
        g->inc[v * ne + e] = -1;
        g->w[e] = w;
    }
    return g;
}

static void graph_print_inc(const Graph *g) {
    printf("Матрица инцидентности (%d x %d):\n", g->nv, g->ne);
    printf("     ");
    for (int e = 0; e < g->ne; e++)
        printf("e%-4d", e);
    printf("\n");
    for (int v = 0; v < g->nv; v++) {
        printf("v%-3d ", v);
        for (int e = 0; e < g->ne; e++) {
            int c = g->inc[v * g->ne + e];
            if      (c ==  1) printf(" +1  ");
            else if (c == -1) printf(" -1  ");
            else              printf("  0  ");
        }
        printf("\n");
    }
    printf("Веса рёбер: ");
    for (int e = 0; e < g->ne; e++) {
        if (e) printf(", ");
        printf("e%d=%lld", e, g->w[e]);
    }
    printf("\n");
}

/* Преобразует матрицу инцидентности в матрицу расстояний для алгоритма */
static long long *build_dist(const Graph *g) {
    int n = g->nv;
    long long *dist = (long long *)malloc(n * n * sizeof(long long));

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            dist[i * n + j] = (i == j) ? 0 : INF;

    for (int e = 0; e < g->ne; e++) {
        int src = -1, dst = -1;
        for (int v = 0; v < n; v++) {
            if      (g->inc[v * g->ne + e] ==  1) src = v;
            else if (g->inc[v * g->ne + e] == -1) dst = v;
        }
        if (src != -1 && dst != -1)
            dist[src * n + dst] = g->w[e];
    }
    return dist;
}

/* ── Алгоритм Флойда-Уоршела ── */

static void floyd_warshall(int n, long long *d) {
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            if (d[i * n + k] >= INF) continue;
            for (int j = 0; j < n; j++) {
                if (d[k * n + j] >= INF) continue;
                long long через_k = d[i * n + k] + d[k * n + j];
                if (через_k < d[i * n + j])
                    d[i * n + j] = через_k;
            }
        }
    }
}

static int has_neg_cycle(int n, const long long *d) {
    for (int i = 0; i < n; i++)
        if (d[i * n + i] < 0) return 1;
    return 0;
}

static void print_dist(int n, const long long *d) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (j) printf(" ");
            if (d[i * n + j] >= INF) printf("INF");
            else printf("%lld", d[i * n + j]);
        }
        printf("\n");
    }
}

/* ── main ── */

#ifndef UNIT_TEST
int main(int argc, char *argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    FILE *in = stdin;
    if (argc >= 2) {
        in = fopen(argv[1], "r");
        if (!in) {
            fprintf(stderr, "Ошибка: не удалось открыть '%s'\n", argv[1]);
            return 1;
        }
    }

    Graph *g = graph_read(in);
    if (in != stdin) fclose(in);

    if (!g) {
        fprintf(stderr, "Ошибка чтения графа. "
                        "Формат: n m, затем строки 'u v w'\n");
        return 1;
    }

    graph_print_inc(g);
    printf("\n");

    long long *dist = build_dist(g);
    floyd_warshall(g->nv, dist);

    if (has_neg_cycle(g->nv, dist)) {
        printf("В графе обнаружен отрицательный цикл!\n");
    } else {
        printf("Матрица кратчайших расстояний (алгоритм Флойда-Уоршела):\n");
        print_dist(g->nv, dist);
    }

    free(dist);
    graph_free(g);
    return 0;
}
#endif
