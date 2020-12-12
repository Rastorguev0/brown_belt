#pragma once
#include "json.h"
#include <map>
#include <string>
#include <vector>
#include <tuple>
#include <utility>
using namespace std;

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


struct GetBusInfo {
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


struct Item {
	string type;
	double time;
};

struct Wait : Item {
	Wait(string name_, double time_) {
		type = "Wait";
		name = move(name_);
		time = move(time_);
	}
	string name;
};

using ItemPtr = unique_ptr<Item>;

struct Bus : Item {
	Bus(string bus_, int spans_, double time_) {
		type = "Bus";
		bus = move(bus_);
		spans = spans_;
		time = time_;
	}
	string bus;
	int spans;
};

struct GetRouteInfo {
	int req_id = 0;
	double total_time = 0;
	vector<ItemPtr> items;
	bool found = false;
};

Json::Node NodeFromStop(GetStopInfo info);
Json::Node NodeFromBus(GetBusInfo info);
Json::Node NodeFromItem(ItemPtr item);
Json::Node NodeFromRoute(GetRouteInfo info);