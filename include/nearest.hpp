#pragma once
#include <limits>
#include "graph_types.hpp"
#include "geo.hpp"

// Returns the ID of the graph vertex closest to (lat, lon).
// If dist_out is given, the distance in metres is written there.
inline int64_t nearest_node(const Graph& g, double lat, double lon, double* dist_out = nullptr) {
    int64_t best = -1;
    double best_d = std::numeric_limits<double>::max();

    for (const auto& entry : g.nodes) {
        const Node& n = entry.second;
        double d = haversine(lat, lon, n.lat, n.lon);
        if (d < best_d) {
            best_d = d;
            best = n.id;
        }
    }

    if (dist_out) *dist_out = best_d;
    return best;
}

