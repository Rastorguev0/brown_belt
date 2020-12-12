#pragma once
#include <optional>
#include <memory>
#include <vector>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <string_view>
#include <utility>
#include "graph.h"
#include "router.h"
#include "responses.h"

using namespace std;


struct EdgeInfo {
	string type;
	string_view name;
	double time;
};

class TransportGuider;
struct StopInfo;
struct BusInfo;
struct TransportGuider::Settings;

struct Knot {
	size_t id; //мб ненужное поле
	unordered_set<size_t> vertexes;
};

class TransportGraph {
private:
	using Stops = unordered_map<string, StopInfo>;
	using Buses = unordered_map<string, BusInfo>;
public:
	TransportGraph(const Stops& s, const Buses& b, const TransportGuider::Settings& set)
		:  stops_(s), buses_(b), config_(set) {}

	bool GraphExist() const;
	size_t StopId(string_view stop) const;
	auto BuildRoute(size_t from, size_t to) const;
	void ReleaseRouteCache(uint64_t id) const;
	pair<double, vector<ItemPtr>> CreateItems(Graph::Router<double>::RouteInfo info) const;
	void Create();

private:
	size_t AllStopsCount() const;
	void CreateWaitEdge(size_t from, size_t to);
	void CreateBusEdge(size_t from, size_t to, string_view bus_name);
	void TieToKnot(const unordered_set<size_t>& other, size_t v);
	size_t NewVertex(string_view v);
	size_t LastVertex() const;
	double Length(size_t from, size_t to) const;
private:
	size_t last_id =  -1;
	size_t last_bus_edge_to_id;
private:
	const Stops& stops_;
	const Buses& buses_;
	const TransportGuider::Settings& config_;

	optional<Graph::DirectedWeightedGraph<double>> graph;
	mutable unique_ptr<Graph::Router<double>> router_ptr;

	unordered_map<string_view, Knot> knots_;
	vector<string_view> id_to_knot;
	deque<EdgeInfo> edges_;
};