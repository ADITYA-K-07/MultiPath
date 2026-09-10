# MultiPath 🗺️

A C++ map route finder built on real-world OpenStreetMap data — computes shortest paths between two points on an actual road network and benchmarks multiple pathfinding algorithms head-to-head.

Built as a Data Structures course project to explore graph representations, priority queues, and empirical algorithm analysis on real geographic data (not toy graphs).

---

## ✨ Features

- Real road network data parsed from OpenStreetMap (`.osm.pbf`) extracts
- Multiple shortest-path algorithms implemented and benchmarked:
  - Dijkstra's algorithm
  - A* search (haversine-distance heuristic)
  - Bidirectional Dijkstra
  - *(optional)* Bellman-Ford
  - *(optional)* Contraction Hierarchies
- Performance comparison across runtime, nodes expanded, and queue size
- Route visualization on a real map (GeoJSON + Leaflet.js)
- Algorithm comparison charts

---

## 🛠️ Tech Stack

| Component | Tool |
|---|---|
| Core language | C++17 |
| Build system | CMake |
| OSM parsing | libosmium / osmium-tool |
| Map data | OpenStreetMap extracts (via Geofabrik) |
| Route visualization | Leaflet.js (GeoJSON) |
| Performance charts | Python (matplotlib/pandas) |

---

## 📂 Project Structure

```
MultiPath/
├── src/
│   ├── graph/          # Graph, Node, Edge data structures
│   ├── parser/         # OSM data parsing into graph
│   ├── algorithms/     # Dijkstra, A*, Bidirectional Dijkstra, etc.
│   └── main.cpp
├── data/                # OSM extracts (not committed — see below)
├── viz/
│   ├── map_viewer.html  # Leaflet.js route viewer
│   └── charts.py        # Algorithm comparison charts
├── results/             # CSV output from benchmark runs
├── CMakeLists.txt
└── README.md
```

---

## 🚀 Getting Started

### Prerequisites
- C++17-compatible compiler (GCC/Clang)
- CMake ≥ 3.15
- [libosmium](https://osmcode.org/libosmium/)
- Python 3 + `matplotlib`/`pandas` (optional, for charts)

### Build

```bash
git clone https://github.com/<your-username>/MultiPath.git
cd MultiPath
mkdir build && cd build
cmake ..
make
```

### Get map data

Download a region extract (start small — a single city) from [Geofabrik](https://download.geofabrik.de/) in `.osm.pbf` format and place it in `data/`.

### Run

```bash
./multipath --map data/<your-region>.osm.pbf --from "<lat,lon>" --to "<lat,lon>"
```

This runs all implemented algorithms on the given query, prints the resulting path and cost, and logs performance metrics to `results/`.

---

## 📊 Benchmarking

Each algorithm run is logged with:
- Runtime
- Nodes/edges expanded
- Max priority queue size
- Final path cost

Run `viz/charts.py` on the generated CSV to produce comparison plots (e.g., runtime vs. query distance), showing how algorithms like A* and Bidirectional Dijkstra outperform plain Dijkstra as distance grows.

---

## 🗺️ Visualizing a Route

After running a query, open `viz/map_viewer.html` in a browser — it loads the most recent route GeoJSON and renders it on a real map.

---

## 📈 Sample Results

*(Add screenshots/GIFs of the map viewer and comparison charts here once available.)*

---

## 🧭 Roadmap

- [x] OSM parsing → graph construction
- [x] Dijkstra
- [x] A*
- [x] Bidirectional Dijkstra
- [ ] Bellman-Ford (contrast)
- [ ] Contraction Hierarchies (stretch goal)
- [ ] Web-based route viewer polish

---

## 📄 License

MIT

---

## 🙋 Author

Aditya Katare
Aryan Khade
