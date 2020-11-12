#pragma once
#include "input_parsing.h"
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <string>
using namespace std;

double Length(const Coordinates& from, const Coordinates& to);

struct StopInfo {
	StopInfo() = default;
	StopInfo(Coordinates c) : coords(c) {}
	bool operator== (const StopInfo& other) const = default;
	Coordinates coords;
};

ostream& operator<<(ostream& os, const StopInfo& si);

struct BusInfo {
	BusInfo() = default;
	BusInfo(vector<string> stops_, bool circled)
		: stops(move(stops_)), is_circled(circled) {}
	vector<string> stops;
	bool is_circled;
	bool operator== (const BusInfo& other) const = default;
};

struct GetBusInfo {
	//bus not found = all_stops_count == 0
	GetBusInfo() = default;
	GetBusInfo(string id, size_t stops, size_t u_stops, double l)
		: bus_id(move(id)), all_stops_count(stops),
		unique_stops_count(u_stops), length(l) {}
	string bus_id;
	size_t all_stops_count;
	size_t unique_stops_count;
	double length;
};

ostream& operator<<(ostream& os, const BusInfo& bi);

class TransportGuider {
public:
	TransportGuider() = default;
	void ProcessQueries(vector<QueryPtr> queries, ostream& stream = cout);
	void ProcessStopQuery(StopQuery& query);
	void ProcessBusStopsQuery(BusStopsQuery& query);
	vector<GetBusInfo> ProcessGetBusInfoQuery(GetBusInfoQuery& query);
	void BusInfoOutput(const vector<GetBusInfo> buses_info, ostream& stream = cout);

	const unordered_map<string, StopInfo>& CheckStops() const;
	const unordered_map<string, BusInfo>& CheckBuses() const;
protected:
	size_t UniqueStopsCount(vector<string>& stops) const;
	double GetLength(vector<string>& stops) const;
protected:
	unordered_map<string, StopInfo> stops_info;
	unordered_map<string, BusInfo> buses_info;
};