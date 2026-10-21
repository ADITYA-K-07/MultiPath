#pragma once
#include <vector>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include "graph_types.hpp"
#include "geo.hpp"
#include "dijkstra.hpp"   // for SearchResult

inline SearchResult astar(const Graph& g, int64_t start, int64_t goal) {
    SearchResult res;

    const Node& goal_node = g.nodes.at(goal);
    auto h = [&](int64_t id) {
        const Node& n = g.nodes.at(id);
        return haversine(n.lat, n.lon, goal_node.lat, goal_node.lon);
    };

    // (f = g + h, g, vertex): ordered by f, smallest first
    using QItem = std::tuple<double, double, int64_t>;
    std::priority_queue<QItem, std::vector<QItem>, std::greater<QItem>> pq;

    std::unordered_map<int64_t, double> dist;      // g values
    std::unordered_map<int64_t, int64_t> parent;

    dist[start] = 0.0;
    pq.push({h(start), 0.0, start});

    while (!pq.empty()) {
        res.max_queue = std::max(res.max_queue, pq.size());

        auto [f, gd, u] = pq.top();
        pq.pop();

        if (gd > dist[u]) continue;        // stale entry

        ++res.nodes_expanded;

        if (u == goal) {
            res.found = true;
            res.cost = gd;
            break;
        }

        auto it = g.adjacency.find(u);
        if (it == g.adjacency.end()) continue;

        for (const Edge& e : it->second) {
            double ng = gd + e.weight;
            auto dit = dist.find(e.target);
            if (dit == dist.end() || ng < dit->second) {
                dist[e.target] = ng;
                parent[e.target] = u;
                pq.push({ng + h(e.target), ng, e.target});
            }
        }
    }

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

