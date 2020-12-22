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


struct Settings {
	double time = 0; // min
	double velocity = 0; // km/min
};

struct Knot {
	Graph::VertexId in;
	Graph::VertexId out;
};

class TransportGraph {
private:
	using Stops = unordered_map<string, StopInfo>;
	using Buses = unordered_map<string, BusInfo>;
	using DoubleGraph = Graph::DirectedWeightedGraph<double>;
	using Router = Graph::Router<double>;
public:
	TransportGraph(
		const Stops& s, const Buses& b, const Settings& set
	) : stops_(s), buses_(b), config_(set) {}

	bool GraphExist() const;
	Graph::VertexId StopId(string_view stop) const;
	optional<GetRouteInfo> BuildRoute(Graph::VertexId from, Graph::VertexId to) const;
	void Create();

private:
	double Length(size_t from, size_t to) const;
	void FillWithStops();
	void FillWithBuses();
	void AddEdge(string_view name, Graph::VertexId from, Graph::VertexId to, double time, size_t spans);
private:
	const Stops& stops_;
	const Buses& buses_;
	const Settings& config_;

	optional<DoubleGraph> graph;
	unique_ptr<Router> router_ptr;

	unordered_map<string_view, Knot> knots_;
	vector<string_view> id_to_stop_;
	deque<ItemPtr> edges_;
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