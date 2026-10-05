#include <osmium/io/pbf_input.hpp>
#include <osmium/io/reader.hpp>
#include <iostream>

int main() {
    osmium::io::Reader reader{"data/pune-highways.osm.pbf"};
    std::cout << "opened ok\n";
}
