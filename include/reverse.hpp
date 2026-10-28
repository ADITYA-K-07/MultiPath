#pragma once
#include "graph_types.hpp"

inline void build_reverse(Graph& g) {
    for (const auto& [u, edges] : g.adjacency) {
        for (const Edge& e : edges) {
		g.reverse_adjacency[e.target].push_back(Edge{u, e.weight});
        }
    }
}
