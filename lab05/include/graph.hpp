#pragma once
#include <vector>
#include <limits>
#include <stdexcept>
#include <string>
#include <iostream>
#include <iomanip>

class Graph {
public:
    static constexpr double INF = std::numeric_limits<double>::infinity();

    explicit Graph(int n);

    void addEdge(int u, int v, double weight);

    int numVertices() const { return n_; }
    int numEdges()    const { return m_; }

    std::vector<std::vector<double>> buildAdjMatrix() const;

    void printIncidenceMatrix() const;

private:
    int n_;
    int m_;
    std::vector<std::vector<int>> inc_;
    std::vector<double> weights_;
};
