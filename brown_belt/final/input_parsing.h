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

enum class QueryType {
	BUS_STOPS,
	STOP,
	GET_STOP_INFO,
	GET_BUS_INFO,
	SETTINGS,
	ROUTE
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

struct SettingsQuery : Query {
	SettingsQuery(double t, double v) : w_time(t), b_vel(v)
	{
		type = QueryType::SETTINGS;
	}
	double w_time; //min
	double b_vel; //km/min
};

using Distances = unordered_map<string, double>;
struct StopQuery : Query {
	StopQuery(const string& stop, double lat_, double long_, const Distances& dist)
		: stop_name(stop), coords({ lat_, long_ }), distances(dist)
	{
		type = QueryType::STOP;
	}

	string stop_name;
	Distances distances;
	Coordinates coords;
};

struct GetStopInfoQuery : Query {
	GetStopInfoQuery(const string& name, int id) : stop_name(name)
	{
		type = QueryType::GET_STOP_INFO;
		req_id = id;
	}

	string stop_name;
};

struct BusStopsQuery : Query {
	BusStopsQuery(const string& id, const vector<string>& stops_, bool circled)
		: bus_id(id), stops(stops_), is_circled(circled)
	{
		type = QueryType::BUS_STOPS;
	}

	string bus_id;
	vector<string> stops;
	bool is_circled;
};

struct GetBusInfoQuery : Query {
	GetBusInfoQuery(const string& id, int r_id) : bus_id(id)
	{
		type = QueryType::GET_BUS_INFO;
		req_id = r_id;
	}

	string bus_id;
};

struct RouteQuery : Query {
	RouteQuery(const string& f, const string& t, int id)
		: from(f), to(t)
	{
		type = QueryType::ROUTE;
		req_id = id;
	}
	string from, to;
};

using QueryPtr = unique_ptr<Query>;

QueryPtr ParsePutQuery(const Json::Node& query);

QueryPtr ParseGetQuery(const Json::Node& query);

vector<QueryPtr> ReadQueries(istream& input = cin);

SettingsQuery* SetCast(Query& query);

StopQuery* StopCast(Query& query);

GetStopInfoQuery* StopGetCast(Query& query);

BusStopsQuery* BusStopsCast(Query& query);

GetBusInfoQuery* BusGetCast(Query& query);

RouteQuery* RouteCast(Query& query);