#pragma once
#include "input_parsing.h"
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <string>
using namespace std;

struct StopInfo {
	StopInfo(Coordinates c) : coords(c) {}
	Coordinates coords;
};

struct BusInfo {
	BusInfo(vector<string> stops_, bool circled)
		: stops(move(stops_)), is_circled(circled) {}
	vector<string> stops;
	bool is_circled;
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

class TransportGuider {
public:
	TransportGuider() = default;
	void ProcessQueries(vector<QueryPtr> queries, ostream& stream = cout);
	void ProcessStopQuery(StopQuery& query);
	void ProcessBusStopsQuery(BusStopsQuery& query);
	vector<GetBusInfo> ProcessGetBusInfoQuery(GetBusInfoQuery& query);
	void BusInfoOutput(const vector<GetBusInfo> buses_info, ostream& stream = cout);
protected:
	size_t UniqueStopsCount(vector<string>& stops) const;
	double GetLength(vector<string>& stops) const;
	double Length(Coordinates from, Coordinates to) const;
protected:
	unordered_map<string, StopInfo> stops_info;
	unordered_map<string, BusInfo> buses_info;
};