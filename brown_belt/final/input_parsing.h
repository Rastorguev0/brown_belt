#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <sstream>
#include <stdexcept>
#include <memory>
#include <algorithm>
#include <tuple>
#include <unordered_map>
#include "json.h"
using namespace std;

//const vector<char> DELIMETERS = { '-', '>' };

enum class QueryType {
	BUS_STOPS,
	STOP,
	GET_STOP_INFO,
	GET_BUS_INFO,
};

struct Query {
	Query() = default;
	QueryType type;
	int req_id = 0;
	virtual QueryType Type() const { return type; }
};

struct Coordinates {
	double latitude = 0.0;
	double longitude = 0.0;

	double LatRad() const;
	double LongRad() const;

	bool operator==(const Coordinates& other) const {
		return make_tuple(latitude, longitude)
			== make_tuple(other.latitude, other.longitude);
	}
};

//ostream& operator<<(ostream& os, const Coordinates& c);

using Distances = unordered_map<string, double>;
struct StopQuery : Query {
	StopQuery(string stop, double lat_, double long_, Distances dist)
		: stop_name(move(stop)), coords({ lat_, long_ }), distances(move(dist))
	{
		type = QueryType::STOP;
	}

	bool operator==(const StopQuery& other) const {
		return make_tuple(stop_name, coords, distances)
			== make_tuple(other.stop_name, other.coords, other.distances);
	}


	string stop_name;
	Distances distances;
	Coordinates coords;
};

struct GetStopInfoQuery : Query {
	GetStopInfoQuery(string name, int id) : stop_name(move(name))
	{
		type = QueryType::GET_STOP_INFO;
		req_id = id;
	}

	bool operator== (const GetStopInfoQuery& other) {
		return stop_name == other.stop_name;
	}
	string stop_name;
};

struct BusStopsQuery : Query {
	BusStopsQuery(string id, vector<string> stops_, bool circled)
		: bus_id(move(id)), stops(move(stops_)), is_circled(circled)
	{
		type = QueryType::BUS_STOPS;
	}

	bool operator==(const BusStopsQuery& other) const {
		return make_tuple(bus_id, stops, is_circled)
			== make_tuple(other.bus_id, other.stops, other.is_circled);
	}

	string bus_id;
	vector<string> stops;
	bool is_circled;
};

struct GetBusInfoQuery : Query {
	GetBusInfoQuery(string id, int r_id) : bus_id(move(id))
	{
		type = QueryType::GET_BUS_INFO;
		req_id = r_id;
	}

	bool operator==(const GetBusInfoQuery& other) const {
		return bus_id == other.bus_id;
	}

	string bus_id;
};
/*
template <typename Number>
Number ReadNumberOnLine(istream& stream);

double ConvertToDouble(string_view line);

unsigned ConvertFromMeters(string_view line);
*/
using QueryPtr = unique_ptr<Query>;
/*
void Trim(string_view& view);

const char CheckDelimiterType(string_view line);

string GetSeparatedToken(string_view& line, const char delimeter = ' ');
*/
QueryPtr ParsePutQuery(const Json::Node& query);

QueryPtr ParseGetQuery(const Json::Node& query);

vector<QueryPtr> ReadQueries(istream& input = cin);

StopQuery* StopCast(Query& query);

GetStopInfoQuery* StopGetCast(Query& query);

BusStopsQuery* BusStopsCast(Query& query);

GetBusInfoQuery* BusGetCast(Query& query);