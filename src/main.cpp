#include <osmium/io/pbf_input.hpp>
#include <osmium/handler.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/index/map/flex_mem.hpp>
#include <osmium/visitor.hpp>
#include <osmium/osm/way.hpp>
#include <geo.hpp>
#include <unordered_map>
#include <cstdint>
#include <iostream>

// ---------- Pass 1: count how many times each node is referenced ----------
class RefCounter : public osmium::handler::Handler {
public:
    std::unordered_map<int64_t, int> ref_count;

    void way(const osmium::Way& way) {
        for (const auto& node_ref : way.nodes()) {
            ++ref_count[node_ref.ref()];
        }
    }
};

// ---------- Pass 2 (test): check that node coordinates resolve ----------
class LocationPrinter : public osmium::handler::Handler {
public:
    int printed = 0;

    void way(const osmium::Way& way) {
        if (printed >= 3) return;
        const auto& first = way.nodes().front();
        std::cout << "way " << way.id()
                  << " first node " << first.ref()
                  << " at lat=" << first.location().lat()
                  << " lon=" << first.location().lon() << "\n";
        ++printed;
    }
};

int main() {
    std::cout << "1 degree of latitude = " << haversine(0, 0, 1, 0) << "m\n";
    const char* file = "data/pune-highways.osm.pbf";

    // Pass 1
    {
        osmium::io::Reader reader{file, osmium::osm_entity_bits::way};
        RefCounter counter;
        osmium::apply(reader, counter);
        reader.close();

        std::cout << "distinct nodes referenced: " << counter.ref_count.size() << "\n";
        int shared = 0;
        for (const auto& entry : counter.ref_count) {
            if (entry.second >= 2) ++shared;
        }
        std::cout << "nodes referenced 2+ times: " << shared << "\n";
    }

    // Pass 2 (test)
    {
        using index_type = osmium::index::map::FlexMem<osmium::unsigned_object_id_type, osmium::Location>;
        using location_handler_type = osmium::handler::NodeLocationsForWays<index_type>;

        index_type index;
        location_handler_type location_handler{index};
        LocationPrinter printer;

        osmium::io::Reader reader{file};   // no entity filter: nodes AND ways
        osmium::apply(reader, location_handler, printer);
        reader.close();
    }

    return 0;
}
