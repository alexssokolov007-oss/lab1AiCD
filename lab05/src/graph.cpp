#include "graph.hpp"

Graph::Graph(int n) : n_(n), m_(0), inc_(n) {
    if (n <= 0)
        throw std::invalid_argument("Vertex count must be positive");
}

void Graph::addEdge(int u, int v, double weight) {
    if (u < 0 || u >= n_ || v < 0 || v >= n_)
        throw std::out_of_range("Vertex index out of range");
    if (u == v)
        throw std::invalid_argument("Self-loops are not supported");

    for (int i = 0; i < n_; ++i)
        inc_[i].push_back(0);

    inc_[u][m_] =  1;
    inc_[v][m_] = -1;
    weights_.push_back(weight);
    ++m_;
}

std::vector<std::vector<double>> Graph::buildAdjMatrix() const {
    std::vector<std::vector<double>> adj(n_, std::vector<double>(n_, INF));
    for (int i = 0; i < n_; ++i)
        adj[i][i] = 0.0;

    for (int j = 0; j < m_; ++j) {
        int src = -1, dst = -1;
        for (int i = 0; i < n_; ++i) {
            if      (inc_[i][j] ==  1) src = i;
            else if (inc_[i][j] == -1) dst = i;
        }
        if (src != -1 && dst != -1) {
            if (adj[src][dst] == INF || weights_[j] < adj[src][dst])
                adj[src][dst] = weights_[j];
        }
    }
    return adj;
}

void Graph::printIncidenceMatrix() const {
    std::cout << "Incidence matrix (" << n_ << " x " << m_ << "):\n";
    if (m_ == 0) {
        std::cout << "  (no edges)\n";
        return;
    }
    std::cout << std::setw(4) << " ";
    for (int j = 0; j < m_; ++j)
        std::cout << std::setw(4) << ("e" + std::to_string(j));
    std::cout << "\n";
    for (int i = 0; i < n_; ++i) {
        std::cout << std::setw(2) << ("v" + std::to_string(i));
        for (int j = 0; j < m_; ++j)
            std::cout << std::setw(4) << inc_[i][j];
        std::cout << "\n";
    }
}
