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
using namespace std;

const vector<char> DELIMETERS = { '-', '>' };

enum class QueryType {
	BUS_STOPS,
	STOP,
	GET_BUS_INFO,
};

struct Query {
	Query() = default;
	QueryType type;
};

struct Coordinates {
	double latitude = 0.0;
	double longitude = 0.0;

	double LatRad() const {
		return latitude * 3.1415926535 / 180;
	}
	double LongRad() const {
		return longitude * 3.1415926535 / 180;
	}

	bool operator==(const Coordinates& other) const {
		return make_tuple(latitude, longitude)
			== make_tuple(other.latitude, other.longitude);
	}
};

ostream& operator<<(ostream& os, const Coordinates& c);

struct StopQuery : Query {
	StopQuery(string_view line);

	StopQuery(string stop, double lat_, double long_)
		: stop_name(move(stop)), coords({ lat_, long_ }) {
		type = QueryType::STOP;
	}

	bool operator==(const StopQuery& other) const {
		return make_tuple(stop_name, coords)
			== make_tuple(other.stop_name, other.coords);
	}

	string stop_name;
	Coordinates coords;
};

struct BusStopsQuery : Query {
	BusStopsQuery(string_view line);

	BusStopsQuery(string id, vector<string> stops_, bool circled)
		: bus_id(move(id)), stops(move(stops_)), is_circled(circled) {
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
	GetBusInfoQuery(string_view line);

	GetBusInfoQuery(string id) : bus_id(move(id)) {
		type = QueryType::GET_BUS_INFO;
	}

	bool operator==(const GetBusInfoQuery& other) const {
		return bus_id == other.bus_id;
	}

	string bus_id;
};

template <typename Number>
Number ReadNumberOnLine(istream& stream);

double ConvertToDouble(string_view line);

using QueryPtr = unique_ptr<Query>;

void Trim(string_view& view);

const char CheckDelimiterType(string_view line);

string GetSeparatedToken(string_view& line, const char delimeter = ' ');

QueryPtr ParsePutQuery(string_view line);

QueryPtr ParseGetQuery(string_view line);

vector<QueryPtr> ReadQueries(istream& input = cin);

StopQuery* StopCast(Query& query);

BusStopsQuery* BusStopsCast(Query& query);

GetBusInfoQuery* BusGetCast(Query& query);