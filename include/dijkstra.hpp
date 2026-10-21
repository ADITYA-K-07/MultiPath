#pragma once
#include <vector>
#include <queue>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <utility>
#include "graph_types.hpp"

struct SearchResult {
    bool found = false;
    double cost = 0.0;                 // total path length in metres
    std::vector<int64_t> path;         // vertex IDs from start to goal
    size_t nodes_expanded = 0;         // metrics for Phase 5
    size_t max_queue = 0;
};

inline SearchResult dijkstra(const Graph& g, int64_t start, int64_t goal) {
    SearchResult res;

    using QItem = std::pair<double, int64_t>;   // (distance so far, vertex)
    std::priority_queue<QItem, std::vector<QItem>, std::greater<QItem>> pq;

    std::unordered_map<int64_t, double> dist;      // best known distance to each vertex
    std::unordered_map<int64_t, int64_t> parent;   // how we reached each vertex

    dist[start] = 0.0;
    pq.push({0.0, start});

    while (!pq.empty()) {
        res.max_queue = std::max(res.max_queue, pq.size());

        auto [d, u] = pq.top();
        pq.pop();

        if (d > dist[u]) continue;     // stale entry: a shorter route to u was already found

        ++res.nodes_expanded;

        if (u == goal) {
            res.found = true;
            res.cost = d;
            break;
        }

        auto it = g.adjacency.find(u);
        if (it == g.adjacency.end()) continue;   // vertex with no outgoing edges

        for (const Edge& e : it->second) {
            double nd = d + e.weight;                 // candidate distance to e.target
            auto dit = dist.find(e.target);
            if (dit == dist.end() || nd < dit->second) {
                dist[e.target] = nd;                  // better route found
                parent[e.target] = u;                 // remember how we got here
                pq.push({nd, e.target});              // (re)queue with the new distance
            }
        }
    }

    // Rebuild the path by walking parents backwards from the goal
    if (res.found) {
        int64_t cur = goal;
        while (cur != start) {
            res.path.push_back(cur);
            cur = parent.at(cur);
        }
        res.path.push_back(start);
        std::reverse(res.path.begin(), res.path.end());
    }

    return res;
}
