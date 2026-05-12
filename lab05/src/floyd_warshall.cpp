#include "floyd_warshall.hpp"

FWResult floydWarshall(const Graph& g) {
    const int n = g.numVertices();
    auto dist = g.buildAdjMatrix();

    std::vector<std::vector<int>> next(n, std::vector<int>(n, -1));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if (i != j && dist[i][j] < Graph::INF)
                next[i][j] = j;

    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            if (dist[i][k] == Graph::INF) continue;
            for (int j = 0; j < n; ++j) {
                if (dist[k][j] == Graph::INF) continue;
                double via = dist[i][k] + dist[k][j];
                if (via < dist[i][j]) {
                    dist[i][j] = via;
                    next[i][j] = next[i][k];
                }
            }
        }
    }

    bool hasNegCycle = false;
    for (int i = 0; i < n; ++i)
        if (dist[i][i] < 0.0) { hasNegCycle = true; break; }

    return {dist, next, hasNegCycle};
}

std::vector<int> reconstructPath(const FWResult& res, int u, int v) {
    if (res.next[u][v] == -1) return {};
    std::vector<int> path = {u};
    while (u != v) {
        u = res.next[u][v];
        path.push_back(u);
    }
    return path;
}
