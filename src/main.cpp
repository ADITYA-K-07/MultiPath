#include <osmium/io/pbf_input.hpp>
#include <osmium/handler.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/index/map/flex_mem.hpp>
#include <osmium/visitor.hpp>
#include <osmium/osm/way.hpp>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <cstring>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <nearest.hpp>
#include "geo.hpp"
#include "graph_types.hpp"

// Road types a car can't use. Skipped in BOTH passes so they stay consistent.
bool is_driveable(const char* highway) {
    static const std::unordered_set<std::string> skip = {
        "footway", "cycleway", "path", "pedestrian", "steps", "bridleway",
        "corridor", "platform", "proposed", "construction", "elevator", "raceway"
    };
    return skip.count(highway) == 0;
}

// ---------- Pass 1: count how many times each node is referenced ----------
class RefCounter : public osmium::handler::Handler {
public:
    std::unordered_map<int64_t, int> ref_count;

    void way(const osmium::Way& way) {
        const char* hw = way.tags()["highway"];
        if (!hw || !is_driveable(hw)) return;

        for (const auto& node_ref : way.nodes()) {
            ++ref_count[node_ref.ref()];
        }
    }
};

// ---------- Pass 2: build the graph ----------
class GraphBuilder : public osmium::handler::Handler {
    const std::unordered_map<int64_t, int>& ref_count;
    Graph& graph;

public:
    GraphBuilder(const std::unordered_map<int64_t, int>& rc, Graph& g)
        : ref_count(rc), graph(g) {}

    void way(const osmium::Way& way) {
        const char* hw = way.tags()["highway"];
        if (!hw || !is_driveable(hw)) return;
        if (way.nodes().size() < 2) return;

        // Which directions can you drive along this way?
        bool forward = true;
        bool backward = true;
        const char* ow = way.tags()["oneway"];
        const char* junction = way.tags()["junction"];
        if (ow) {
            if (!std::strcmp(ow, "yes") || !std::strcmp(ow, "true") || !std::strcmp(ow, "1")) {
                backward = false;
            } else if (!std::strcmp(ow, "-1")) {
                forward = false;
            }
        } else if (junction && !std::strcmp(junction, "roundabout")) {
            backward = false;
        }

        // Walk along the way, folding shape points into one edge per span.
        const auto& first = way.nodes().front();
        int64_t last_vertex = first.ref();
        graph.nodes[last_vertex] = Node{last_vertex, first.location().lat(), first.location().lon()};

        double acc = 0.0;
        double prev_lat = first.location().lat();
        double prev_lon = first.location().lon();
        const size_t n = way.nodes().size();

        for (size_t i = 1; i < n; ++i) {
            const auto& cur = way.nodes()[i];
            double lat = cur.location().lat();
            double lon = cur.location().lon();

            acc += haversine(prev_lat, prev_lon, lat, lon);
            prev_lat = lat;
            prev_lon = lon;

            bool is_vertex = (i == n - 1) || (ref_count.at(cur.ref()) >= 2);
            if (!is_vertex) continue;

            int64_t id = cur.ref();
            graph.nodes[id] = Node{id, lat, lon};

            if (id != last_vertex) {   // skip self-loops
                if (forward)  graph.adjacency[last_vertex].push_back(Edge{id, acc});
                if (backward) graph.adjacency[id].push_back(Edge{last_vertex, acc});
            }

            last_vertex = id;
            acc = 0.0;
        }
    }
};

int main() {
    const char* file = "data/pune-highways.osm.pbf";

    // Pass 1
    RefCounter counter;
    {
        osmium::io::Reader reader{file, osmium::osm_entity_bits::way};
        osmium::apply(reader, counter);
        reader.close();
    }
    int shared = 0;
    for (const auto& entry : counter.ref_count) {
        if (entry.second >= 2) ++shared;
    }
    std::cout << "distinct nodes referenced: " << counter.ref_count.size() << "\n";
    std::cout << "nodes referenced 2+ times: " << shared << "\n";

    // Pass 2
    Graph graph;
    {
        using index_type = osmium::index::map::FlexMem<osmium::unsigned_object_id_type, osmium::Location>;
        using location_handler_type = osmium::handler::NodeLocationsForWays<index_type>;

        index_type index;
        location_handler_type location_handler{index};
        GraphBuilder builder{counter.ref_count, graph};

        osmium::io::Reader reader{file};   // nodes AND ways
        osmium::apply(reader, location_handler, builder);
        reader.close();
    }

    size_t edge_count = 0;
    for (const auto& entry : graph.adjacency) {
        edge_count += entry.second.size();
    }
    std::cout << "graph vertices: " << graph.nodes.size() << "\n";
    std::cout << "graph edges (directed): " << edge_count << "\n";

    // ---- Sanity checks ----
    std::unordered_map<int, int> degree_hist;
    for (const auto& entry : graph.nodes) {
        auto it = graph.adjacency.find(entry.first);
        int deg = (it == graph.adjacency.end()) ? 0 : (int)it->second.size();
        ++degree_hist[deg];
    }
    for (int d = 0; d <= 6; ++d) {
        std::cout << "out-degree " << d << ": " << degree_hist[d] << "\n";
    }
    double total = 0.0, longest = 0.0;
    int64_t long_from = 0, long_to = 0;
    for (const auto& entry : graph.adjacency) {
        for (const auto& e : entry.second) {
            total += e.weight;
            if (e.weight > longest) {
                longest = e.weight;
                long_from = entry.first;
                long_to = e.target;
            }
        }
    }
    std::cout << "total edge length (km): " << total / 1000.0 << "\n";
    std::cout << "longest single edge (m): " << longest << "\n";

    const Node& a = graph.nodes.at(long_from);
    const Node& b = graph.nodes.at(long_to);
    std::cout << std::setprecision(7);
    std::cout << "from: https://www.google.com/maps?q=" << a.lat << "," << a.lon << "\n";
    std::cout << "to:   https://www.google.com/maps?q=" << b.lat << "," << b.lon << "\n";
    // ---- Nearest-node test ----
    struct Place { const char* name; double lat; double lon; };
    Place places[] = {
        {"Shaniwar Wada", 18.5195, 73.8553},
        {"Pune Railway Station", 18.5289, 73.8744},
        {"Middle of nowhere (sanity)", 18.4300, 73.7600}
    };
    for (const auto& p : places) {
        double d = 0.0;
        int64_t id = nearest_node(graph, p.lat, p.lon, &d);
        const Node& n = graph.nodes.at(id);
        std::cout << p.name << " -> node " << id
                  << " (" << n.lat << ", " << n.lon << "), "
                  << d << " m away\n";
    }

    return 0;
}
