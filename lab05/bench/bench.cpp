#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <random>
#include "graph.hpp"
#include "floyd_warshall.hpp"

static double measure(int n, int edgeCount, unsigned seed = 42) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> vd(0, n - 1);
    std::uniform_real_distribution<double> wd(1.0, 100.0);

    Graph g(n);
    int added = 0;
    for (int attempt = 0; attempt < edgeCount * 4 && added < edgeCount; ++attempt) {
        int u = vd(rng), v = vd(rng);
        if (u == v) continue;
        g.addEdge(u, v, wd(rng));
        ++added;
    }

    auto t0 = std::chrono::high_resolution_clock::now();
    floydWarshall(g);
    auto t1 = std::chrono::high_resolution_clock::now();

    return std::chrono::duration<double, std::milli>(t1 - t0).count();
}

int main() {
    std::cout << "Floyd-Warshall Benchmark\n\n";

    struct Case { int n; int edges; };
    std::vector<Case> cases = {
        {  10,    20},
        {  50,   200},
        { 100,   500},
        { 200,  1500},
        { 500,  5000},
        {1000, 15000},
    };

    const int RUNS = 5;

    std::cout << std::left
              << std::setw(10) << "Vertices"
              << std::setw(10) << "Edges"
              << std::setw(16) << "Avg time (ms)"
              << "\n"
              << std::string(36, '-') << "\n";

    for (auto& c : cases) {
        double total = 0.0;
        for (int r = 0; r < RUNS; ++r)
            total += measure(c.n, c.edges, 42u + r);
        double avg = total / RUNS;
        std::cout << std::setw(10) << c.n
                  << std::setw(10) << c.edges
                  << std::setw(16) << std::fixed << std::setprecision(3) << avg
                  << "\n";
    }

    return 0;
}
