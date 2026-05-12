#include <gtest/gtest.h>
#include "graph.hpp"
#include "floyd_warshall.hpp"

TEST(FloydWarshall, BasicShortestDistances) {
    Graph g(4);
    g.addEdge(0, 1, 3.0);
    g.addEdge(1, 2, 2.0);
    g.addEdge(2, 3, 1.0);
    g.addEdge(0, 2, 6.0);
    g.addEdge(1, 3, 8.0);

    auto res = floydWarshall(g);

    ASSERT_FALSE(res.hasNegativeCycle);
    EXPECT_DOUBLE_EQ(res.dist[0][1], 3.0);
    EXPECT_DOUBLE_EQ(res.dist[0][2], 5.0);
    EXPECT_DOUBLE_EQ(res.dist[0][3], 6.0);
    EXPECT_DOUBLE_EQ(res.dist[1][3], 3.0);
}

TEST(FloydWarshall, DisconnectedVertices) {
    Graph g(3);
    g.addEdge(0, 1, 5.0);

    auto res = floydWarshall(g);

    ASSERT_FALSE(res.hasNegativeCycle);
    EXPECT_EQ(res.dist[0][2], Graph::INF);
    EXPECT_EQ(res.dist[1][2], Graph::INF);
    EXPECT_EQ(res.dist[2][0], Graph::INF);
    EXPECT_EQ(res.dist[2][1], Graph::INF);
}

TEST(FloydWarshall, NegativeEdgeWeights) {
    Graph g(3);
    g.addEdge(0, 1,  4.0);
    g.addEdge(1, 2, -2.0);
    g.addEdge(0, 2,  5.0);

    auto res = floydWarshall(g);

    ASSERT_FALSE(res.hasNegativeCycle);
    EXPECT_DOUBLE_EQ(res.dist[0][2], 2.0);
    EXPECT_DOUBLE_EQ(res.dist[0][1], 4.0);
}

TEST(FloydWarshall, NegativeCycleDetected) {
    Graph g(3);
    g.addEdge(0, 1,  1.0);
    g.addEdge(1, 2, -3.0);
    g.addEdge(2, 0,  1.0);

    auto res = floydWarshall(g);

    EXPECT_TRUE(res.hasNegativeCycle);
}

TEST(FloydWarshall, PathReconstruction) {
    Graph g(4);
    g.addEdge(0, 1, 1.0);
    g.addEdge(1, 3, 1.0);
    g.addEdge(0, 2, 2.0);
    g.addEdge(2, 3, 2.0);

    auto res = floydWarshall(g);

    ASSERT_FALSE(res.hasNegativeCycle);
    EXPECT_DOUBLE_EQ(res.dist[0][3], 2.0);

    auto path = reconstructPath(res, 0, 3);
    std::vector<int> expected = {0, 1, 3};
    EXPECT_EQ(path, expected);
}
