#include <gtest/gtest.h>
#include <cstdlib>

extern "C" {
#define UNIT_TEST
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#include "main.c"
#pragma GCC diagnostic pop
}

/* вспомогательная функция */
static long long D(const long long *dist, int n, int i, int j) {
    return dist[i * n + j];
}

/* Тест 1: build_dist корректно переносит рёбра из матрицы инцидентности */
TEST(IncMatrix, BuildDistBasic) {
    /* Граф: 3 вершины, 2 ребра: 0→1 (вес 4), 1→2 (вес 3) */
    Graph *g = graph_new(3, 2);
    g->inc[0 * 2 + 0] =  1;
    g->inc[1 * 2 + 0] = -1;
    g->w[0] = 4;
    g->inc[1 * 2 + 1] =  1;
    g->inc[2 * 2 + 1] = -1;
    g->w[1] = 3;

    long long *dist = build_dist(g);

    EXPECT_EQ(D(dist, 3, 0, 0), 0);
    EXPECT_EQ(D(dist, 3, 0, 1), 4);
    EXPECT_EQ(D(dist, 3, 1, 2), 3);
    EXPECT_EQ(D(dist, 3, 0, 2), INF);  /* прямого ребра нет */

    free(dist);
    graph_free(g);
}

/* Тест 2: Floyd-Warshall находит путь через промежуточную вершину */
TEST(FloydWarshall, ChainRelaxation) {
    /* 0→1(4), 1→2(3): после алгоритма dist[0][2] = 7 */
    Graph *g = graph_new(3, 2);
    g->inc[0*2+0] =  1; g->inc[1*2+0] = -1; g->w[0] = 4;
    g->inc[1*2+1] =  1; g->inc[2*2+1] = -1; g->w[1] = 3;

    long long *dist = build_dist(g);
    floyd_warshall(3, dist);

    EXPECT_EQ(D(dist, 3, 0, 2), 7);    /* 4 + 3 */
    EXPECT_EQ(D(dist, 3, 1, 0), INF);  /* обратного пути нет */
    EXPECT_EQ(has_neg_cycle(3, dist), 0);

    free(dist);
    graph_free(g);
}

/* Тест 3: алгоритм выбирает более короткий путь из двух вариантов */
TEST(FloydWarshall, ShortcutPreferred) {
    /* 0→1(1), 1→2(1), 2→3(1), 0→3(100): путь через 1-2-3 = 3, а не 100 */
    Graph *g = graph_new(4, 4);
    g->inc[0*4+0]=1; g->inc[1*4+0]=-1; g->w[0]=1;
    g->inc[1*4+1]=1; g->inc[2*4+1]=-1; g->w[1]=1;
    g->inc[2*4+2]=1; g->inc[3*4+2]=-1; g->w[2]=1;
    g->inc[0*4+3]=1; g->inc[3*4+3]=-1; g->w[3]=100;

    long long *dist = build_dist(g);
    floyd_warshall(4, dist);

    EXPECT_EQ(D(dist, 4, 0, 3), 3);
    EXPECT_EQ(D(dist, 4, 0, 2), 2);
    EXPECT_EQ(D(dist, 4, 1, 3), 2);

    free(dist);
    graph_free(g);
}

/* Тест 4: нет пути между несвязными компонентами */
TEST(FloydWarshall, NoPath) {
    /* Две несвязные пары: 0→1 и 2→3 */
    Graph *g = graph_new(4, 2);
    g->inc[0*2+0]=1; g->inc[1*2+0]=-1; g->w[0]=5;
    g->inc[2*2+1]=1; g->inc[3*2+1]=-1; g->w[1]=7;

    long long *dist = build_dist(g);
    floyd_warshall(4, dist);

    EXPECT_EQ(D(dist, 4, 0, 1), 5);
    EXPECT_EQ(D(dist, 4, 2, 3), 7);
    EXPECT_EQ(D(dist, 4, 0, 2), INF);
    EXPECT_EQ(D(dist, 4, 1, 3), INF);

    free(dist);
    graph_free(g);
}

/* Тест 5: обнаружение отрицательного цикла */
TEST(FloydWarshall, NegCycleDetected) {
    /* 0→1(1), 1→2(-3), 2→0(-3): сумма цикла = 1 + (−3) + (−3) = −5 */
    Graph *g = graph_new(3, 3);
    g->inc[0*3+0]=1; g->inc[1*3+0]=-1; g->w[0]= 1;
    g->inc[1*3+1]=1; g->inc[2*3+1]=-1; g->w[1]=-3;
    g->inc[2*3+2]=1; g->inc[0*3+2]=-1; g->w[2]=-3;

    long long *dist = build_dist(g);
    floyd_warshall(3, dist);

    EXPECT_EQ(has_neg_cycle(3, dist), 1);

    free(dist);
    graph_free(g);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
