#include <osmium/io/pbf_input.hpp>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>
#include <osmium/osm/way.hpp>
#include <unordered_map>
#include <cstdint>
#include <iostream>

class RefCounter : public osmium::handler::Handler {
public:
    std::unordered_map<int64_t, int> ref_count;

    void way(const osmium::Way& way) {
        for (const auto& node_ref : way.nodes()) {
            ++ref_count[node_ref.ref()];
        }
    }
};

int main() {
    osmium::io::Reader reader{"data/pune-highways.osm.pbf", osmium::osm_entity_bits::way};
    RefCounter counter;
    osmium::apply(reader, counter);
    reader.close();

    std::cout << "distinct nodes referenced: " << counter.ref_count.size() << "\n";

    int shared = 0;
    for (const auto& entry : counter.ref_count) {
        if (entry.second >= 2) ++shared;
    }
    std::cout << "nodes referenced 2+ times: " << shared << "\n";
    return 0;
}
