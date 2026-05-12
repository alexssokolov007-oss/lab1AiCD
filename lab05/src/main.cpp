#include <iostream>
#include <iomanip>
#include <string>
#include "graph.hpp"
#include "floyd_warshall.hpp"

static void printDistMatrix(const FWResult& res, int n) {
    std::cout << "\nMatrix of shortest distances:\n";
    std::cout << std::setw(6) << " ";
    for (int j = 0; j < n; ++j)
        std::cout << std::setw(9) << ("v" + std::to_string(j));
    std::cout << "\n";
    for (int i = 0; i < n; ++i) {
        std::cout << std::setw(4) << ("v" + std::to_string(i));
        for (int j = 0; j < n; ++j) {
            if (res.dist[i][j] == Graph::INF)
                std::cout << std::setw(9) << "INF";
            else
                std::cout << std::setw(9) << std::fixed << std::setprecision(2) << res.dist[i][j];
        }
        std::cout << "\n";
    }
}

static void printPaths(const FWResult& res, int n) {
    std::cout << "\nShortest paths:\n";
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;
            std::cout << "v" << i << " -> v" << j << ":  ";
            if (res.dist[i][j] == Graph::INF) {
                std::cout << "no path\n";
            } else {
                auto path = reconstructPath(res, i, j);
                for (int k = 0; k < static_cast<int>(path.size()); ++k) {
                    if (k) std::cout << " -> ";
                    std::cout << "v" << path[k];
                }
                std::cout << "  (dist = " << res.dist[i][j] << ")\n";
            }
        }
    }
}

int main() {
    std::cout << "Lab 05 (Variant 19): Incidence Matrix + Floyd-Warshall\n\n";

    int n = 0;
    std::cout << "Enter number of vertices: ";
    std::cin >> n;
    if (!std::cin || n <= 0) { std::cerr << "Invalid vertex count.\n"; return 1; }

    Graph g(n);

    int m = 0;
    std::cout << "Enter number of edges: ";
    std::cin >> m;
    if (!std::cin || m < 0) { std::cerr << "Invalid edge count.\n"; return 1; }

    for (int i = 0; i < m; ++i) {
        int u, v; double w;
        std::cout << "Edge " << i << " (from to weight): ";
        std::cin >> u >> v >> w;
        if (!std::cin) { std::cerr << "Invalid edge input.\n"; return 1; }
        try {
            g.addEdge(u, v, w);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
            --i;
        }
    }

    std::cout << "\n";
    g.printIncidenceMatrix();

    auto res = floydWarshall(g);

    if (res.hasNegativeCycle) {
        std::cout << "\n[!] Graph contains a negative-weight cycle. "
                     "Distances are not meaningful.\n";
    } else {
        printDistMatrix(res, n);
        printPaths(res, n);
    }

    return 0;
}
