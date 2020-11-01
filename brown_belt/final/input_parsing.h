#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
using namespace std;

enum class QueryType {
	BUS,
	STOP,
};

QueryType ConvertStrToType(const string& str);

struct Query {
	QueryType type;
};

struct StopQuery : Query {
	StopQuery(double lat_, double long_)
		: latitude(lat_), longitude(long_) {
		type = QueryType::STOP;
	}
	double latitude;
	double longitude;
};

struct BusStopsQuery : Query {

};

struct GetBusInfoQuery : Query {

};

template <typename Number>
Number ReadNumberOnLine(istream& stream);

vector<Query> ReadQueries(istream& input = cin);