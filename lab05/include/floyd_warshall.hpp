#pragma once
#include "graph.hpp"
#include <vector>

struct FWResult {
    std::vector<std::vector<double>> dist; // shortest distances between all pairs
    std::vector<std::vector<int>>    next; // next hop on shortest path (-1 = no path)
    bool hasNegativeCycle;
};

FWResult floydWarshall(const Graph& g);

std::vector<int> reconstructPath(const FWResult& res, int u, int v);
