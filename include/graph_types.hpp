#pragma once
#include <cstdint>
#include <vector>
#include <unordered_map>

struct Node {
	int64_t id;
	double lat;
	double lon;
};

struct Edge {
	int64_t target;
	double weight;
};

struct Graph {
    std::unordered_map<int64_t, Node> nodes;
    std::unordered_map<int64_t, std::vector<Edge>> adjacency;
};



