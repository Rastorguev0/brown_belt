#pragma once
#include "input_parsing.h"
#include "json.h"
#include "graph.h"
#include "router.h"
#include "responses.h"
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <iomanip>
#include <string>
#include <set>
#include <vector>
#include <deque>
#include <string_view>
#include <utility>
#include <optional>
#include <memory>
using namespace std;

double Length(const Coordinates& from, const Coordinates& to);

struct StopInfo {
	StopInfo() = default;
	StopInfo(Coordinates c, set<string> buses_, Distances dist)
		: coords(c), buses(move(buses_)), distances(move(dist)) {}

	Coordinates coords;
	set<string> buses;
	Distances distances;

	bool operator== (const StopInfo& other) const {
		return coords == other.coords && buses == other.buses;
	}
};

struct BusInfo {
	BusInfo() = default;
	BusInfo(vector<string> stops_, bool circled)
		: stops(move(stops_)), is_circled(circled) {}

	vector<string> stops = {};
	bool is_circled = 0;

	bool operator== (const BusInfo& other) const {
		return make_tuple(stops, is_circled) ==
			make_tuple(other.stops, other.is_circled);
	}
};


struct EdgeInfo {
	string type;
	string_view name;
	double time;
};

struct Knot {
	size_t id; //мб ненужное поле
	unordered_set<size_t> vertexes;
};

struct Settings {
	double time = 0;
	double velocity = 0;
};

class TransportGraph {
private:
	using Stops = unordered_map<string, StopInfo>;
	using Buses = unordered_map<string, BusInfo>;
public:
	TransportGraph(const Stops& s, const Buses& b, const Settings& set)
		: stops_(s), buses_(b), config_(set) {}

	bool GraphExist() const;
	size_t StopId(string_view stop) const;
	optional<Graph::Router<double>::RouteInfo> BuildRoute(size_t from, size_t to) const;
	void ReleaseRouteCache(uint64_t id) const;
	pair<double, vector<ItemPtr>> CreateItems(Graph::Router<double>::RouteInfo info) const;
	void Create();
	const unordered_map<string_view, Knot>& CheckKnots() const;

private:
	size_t AllStopsCount() const;
	void CreateWaitEdge(size_t from, size_t to);
	void CreateBusEdge(size_t from, size_t to, string_view bus_name);
	void TieToKnot(const unordered_set<size_t>& other, size_t v);
	size_t NewVertex(string_view v);
	size_t LastVertex() const;
	double Length(size_t from, size_t to) const;
private:
	int last_id = -1;
	int last_bus_edge_to_id = -1;
private:
	const Stops& stops_;
	const Buses& buses_;
	const Settings& config_;

	optional<Graph::DirectedWeightedGraph<double>> graph;
	mutable unique_ptr<Graph::Router<double>> router_ptr;

	unordered_map<string_view, Knot> knots_;
	vector<string_view> id_to_knot;
	deque<EdgeInfo> edges_;
};


class TransportGuider {
public:
	TransportGuider();
	void ProcessQueries(vector<QueryPtr> queries, ostream& stream = cout);
	void SetConfig(SettingsQuery& query);
	void ProcessStopQuery(StopQuery& query);
	GetStopInfo ProcessGetStopInfoQuery(GetStopInfoQuery& query) const;
	void ProcessBusStopsQuery(BusStopsQuery& query);
	GetBusInfo ProcessGetBusInfoQuery(GetBusInfoQuery& query) const;
	GetRouteInfo ProcessGetRouteInfoQuery(RouteQuery& query) const;
	void InfoOutput(const Json::Document& doc, ostream& stream = cout) const;

	const unordered_map<string, StopInfo>& CheckStops() const;
	const unordered_map<string, BusInfo>& CheckBuses() const;
	double RealLength(const string& from, const string& to) const;
	const Settings& CheckSettings() const;
protected:
	size_t UniqueStopsCount(const vector<string>& stops) const;
	double GetLength(const vector<string>& stops) const;
	double GetRealLength(const vector<string>& stops, bool is_circled) const;
protected:
	unordered_map<string, StopInfo> stops_info;
	unordered_map<string, BusInfo> buses_info;
	Settings cfg;
	TransportGraph TG;
};