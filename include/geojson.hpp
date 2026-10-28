#pragma once
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include "graph_types.hpp"

// Writes a path (list of vertex IDs) as a GeoJSON LineString.
inline bool write_geojson(const Graph& g, const std::vector<int64_t>& path,
                          const std::string& filename) {
    std::ofstream out(filename);
    if (!out) return false;

    out << std::fixed << std::setprecision(6);
    out << "{\"type\":\"Feature\",\"properties\":{},"
        << "\"geometry\":{\"type\":\"LineString\",\"coordinates\":[";
    for (size_t i = 0; i < path.size(); ++i) {
        const Node& n = g.nodes.at(path[i]);
        if (i > 0) out << ",";
        out << "[" << n.lon << "," << n.lat << "]";   // GeoJSON order is lon, lat
    }
    out << "]}}\n";
    return true;
}
