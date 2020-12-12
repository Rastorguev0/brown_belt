#pragma once
#include "input_parsing.h"
#include "json.h"
#include "graph.h"
#include "router.h"
#include "responses.h"
#include "graph_creating.h"
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <iomanip>
#include <string>
#include <set>
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


class TransportGuider {
public:
	struct Settings {
		double time = 0;
		double velocity = 0;
	};
public:
	TransportGuider() : TG(TransportGraph{ stops_info, buses_info, cfg }) {};
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