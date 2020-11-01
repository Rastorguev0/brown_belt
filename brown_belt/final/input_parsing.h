#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <memory>
using namespace std;

enum class QueryType {
	BUS_STOPS,
	STOP,
	GET_BUS_INFO,
};

struct Query {
	QueryType type;
};

struct Coordinates {
	double latitude = 0.0;
	double longitude = 0.0;
};
	
struct StopQuery : Query {
	StopQuery(double lat_, double long_)
		: coords({ lat_, long_ }) {
		type = QueryType::STOP;
	}
	Coordinates coords;
};

struct BusStopsQuery : Query {
	BusStopsQuery(string id) : bus_id(id) {
		type = QueryType::BUS_STOPS;
	}
	void AddStop(string stop_name) {
		stops.push_back(move(stop_name));
	}
	string bus_id;
	vector<string> stops;
};

struct GetBusInfoQuery : Query {
	GetBusInfoQuery(string id) : bus_id(id) {
		type = QueryType::GET_BUS_INFO;
	}
	string bus_id;
};

template <typename Number>
Number ReadNumberOnLine(istream& stream);

using QueryPtr = unique_ptr<Query>;

QueryPtr ParseQuery(string_view line);

vector<QueryPtr> ReadQueries(istream& input = cin);