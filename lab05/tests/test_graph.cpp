#include <gtest/gtest.h>
#include <vector>
#include <cmath>
#include "graph.h"
#include "floyd_warshall.h"

TEST(FloydWarshall, BasicShortestDistances) {
    Graph g;
    graph_init(&g, 4);
    graph_add_edge(&g, 0, 1, 3.0);
    graph_add_edge(&g, 1, 2, 2.0);
    graph_add_edge(&g, 2, 3, 1.0);
    graph_add_edge(&g, 0, 2, 6.0);
    graph_add_edge(&g, 1, 3, 8.0);

    FWResult res = floyd_warshall(&g);

    ASSERT_FALSE(res.has_negative_cycle);
    EXPECT_DOUBLE_EQ(res.dist[0 * 4 + 1], 3.0);
    EXPECT_DOUBLE_EQ(res.dist[0 * 4 + 2], 5.0);
    EXPECT_DOUBLE_EQ(res.dist[0 * 4 + 3], 6.0);
    EXPECT_DOUBLE_EQ(res.dist[1 * 4 + 3], 3.0);

    fw_result_free(&res);
    graph_free(&g);
}

TEST(FloydWarshall, DisconnectedVertices) {
    Graph g;
    graph_init(&g, 3);
    graph_add_edge(&g, 0, 1, 5.0);

    FWResult res = floyd_warshall(&g);

    ASSERT_FALSE(res.has_negative_cycle);
    EXPECT_TRUE(std::isinf(res.dist[0 * 3 + 2]));
    EXPECT_TRUE(std::isinf(res.dist[1 * 3 + 2]));
    EXPECT_TRUE(std::isinf(res.dist[2 * 3 + 0]));
    EXPECT_TRUE(std::isinf(res.dist[2 * 3 + 1]));

    fw_result_free(&res);
    graph_free(&g);
}

TEST(FloydWarshall, NegativeEdgeWeights) {
    Graph g;
    graph_init(&g, 3);
    graph_add_edge(&g, 0, 1,  4.0);
    graph_add_edge(&g, 1, 2, -2.0);
    graph_add_edge(&g, 0, 2,  5.0);

    FWResult res = floyd_warshall(&g);

    ASSERT_FALSE(res.has_negative_cycle);
    EXPECT_DOUBLE_EQ(res.dist[0 * 3 + 2], 2.0);
    EXPECT_DOUBLE_EQ(res.dist[0 * 3 + 1], 4.0);

    fw_result_free(&res);
    graph_free(&g);
}

TEST(FloydWarshall, NegativeCycleDetected) {
    Graph g;
    graph_init(&g, 3);
    graph_add_edge(&g, 0, 1,  1.0);
    graph_add_edge(&g, 1, 2, -3.0);
    graph_add_edge(&g, 2, 0,  1.0);

    FWResult res = floyd_warshall(&g);

    EXPECT_TRUE(res.has_negative_cycle);

    fw_result_free(&res);
    graph_free(&g);
}

TEST(FloydWarshall, PathReconstruction) {
    Graph g;
    graph_init(&g, 4);
    graph_add_edge(&g, 0, 1, 1.0);
    graph_add_edge(&g, 1, 3, 1.0);
    graph_add_edge(&g, 0, 2, 2.0);
    graph_add_edge(&g, 2, 3, 2.0);

    FWResult res = floyd_warshall(&g);

    ASSERT_FALSE(res.has_negative_cycle);
    EXPECT_DOUBLE_EQ(res.dist[0 * 4 + 3], 2.0);

    int path_len = 0;
    int *path = reconstruct_path(&res, 0, 3, &path_len);
    std::vector<int> expected = {0, 1, 3};
    ASSERT_EQ(path_len, (int)expected.size());
    for (int i = 0; i < path_len; ++i)
        EXPECT_EQ(path[i], expected[i]);
    free(path);

    fw_result_free(&res);
    graph_free(&g);
}
