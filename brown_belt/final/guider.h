#pragma once
#include "input_parsing.h"
#include "json.h"
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
		: coords(c) , buses(move(buses_)), distances(move(dist)) {}

	Coordinates coords;
	set<string> buses;
	Distances distances;

	bool operator== (const StopInfo& other) const {
		return coords == other.coords && buses == other.buses;
	}
};

ostream& operator<<(ostream& os, const StopInfo& si);

struct GetStopInfo {
	GetStopInfo() = default;
	GetStopInfo(string name, vector<string> buses_, bool found_, double id)
	: stop_name(move(name)), buses(move(buses_)), found(found_), req_id(id) {}

	string stop_name;
	bool found = 0;
	vector<string> buses;
	double req_id;

	bool operator==(const GetStopInfo& other) const {
		return make_tuple(stop_name, found, buses)
			== make_tuple(other.stop_name, other.found, other.buses);
	}
};

Json::Node NodeFromStop(GetStopInfo info);

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

struct GetBusInfo {
	//bus not found = all_stops_count == 0
	GetBusInfo() = default;
	GetBusInfo(string id, size_t stops, size_t u_stops, double l, int rl, bool circled, double r_id)
		: bus_id(move(id)), all_stops_count(stops),
		unique_stops_count(u_stops), length(l), real_length(rl), is_circled(circled), req_id(r_id) {}

	string bus_id;
	size_t all_stops_count = 0;
	size_t unique_stops_count = 0;
	double length = 0;
	double real_length = 0;
	bool is_circled = 0;
	double req_id;

	bool operator== (const GetBusInfo& other) const {
		return make_tuple(bus_id, all_stops_count, unique_stops_count, length, real_length) ==
			make_tuple(other.bus_id, other.all_stops_count, other.unique_stops_count, other.length, other.real_length);
	}
};

Json::Node NodeFromBus(GetBusInfo info);

class TransportGuider {
public:
	TransportGuider() = default;
	void ProcessQueries(vector<QueryPtr> queries, ostream& stream = cout);
	void ProcessStopQuery(StopQuery& query);
	GetStopInfo ProcessGetStopInfoQuery(GetStopInfoQuery& query) const;
	void ProcessBusStopsQuery(BusStopsQuery& query);
	GetBusInfo ProcessGetBusInfoQuery(GetBusInfoQuery& query) const;
	void InfoOutput(const Json::Document& doc, ostream& stream = cout) const;

	const unordered_map<string, StopInfo>& CheckStops() const;
	const unordered_map<string, BusInfo>& CheckBuses() const;
protected:
	size_t UniqueStopsCount(const vector<string>& stops) const;
	double GetLength(const vector<string>& stops) const;
	double GetRealLength(const vector<string>& stops, bool is_circled) const;
	double RealLength(const string& from, const string& to) const;
protected:
	unordered_map<string, StopInfo> stops_info;
	unordered_map<string, BusInfo> buses_info;
};