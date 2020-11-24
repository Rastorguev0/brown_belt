#include "guider.h"
#include <algorithm>
#include <cmath>

const unsigned R = 6371000;

double Length(const Coordinates& lhs, const Coordinates& rhs) {
	return R * acos(
		sin(lhs.LatRad()) * sin(rhs.LatRad()) +
		cos(lhs.LatRad()) * cos(rhs.LatRad()) *
		cos(abs(lhs.LongRad() - rhs.LongRad()))
	);
}

double TransportGuider::RealLength(const string& from, const string& to) const {
	return stops_info.at(from).distances.at(to);
}

void TransportGuider::ProcessQueries(vector<QueryPtr> queries, ostream& stream) {
	vector<Json::Node> nodes;
	for (const auto& query : queries) {
		switch (query->type) {
		case QueryType::STOP:
			ProcessStopQuery(*StopCast(*query));
			break;
		case QueryType::GET_STOP_INFO:
			nodes.push_back(NodeFromStop(ProcessGetStopInfoQuery(*StopGetCast(*query))));
			break;
		case QueryType::BUS_STOPS:
			ProcessBusStopsQuery(*BusStopsCast(*query));
			break;
		case QueryType::GET_BUS_INFO:
			nodes.push_back(NodeFromBus(ProcessGetBusInfoQuery(*BusGetCast(*query))));
			break;
		}
	}
	InfoOutput(Json::Document(Json::Node(move(nodes))), stream);
}

void TransportGuider::ProcessStopQuery(StopQuery& query) {
	auto& this_stop = stops_info[query.stop_name];
	this_stop.coords = query.coords;
	for (const auto& [stop, dist] : query.distances) {
		this_stop.distances[stop] = dist;
		if (!stops_info[stop].distances.count(query.stop_name)) {
			stops_info[stop].distances[query.stop_name] = dist;
		}
	}
}

GetStopInfo TransportGuider::ProcessGetStopInfoQuery(GetStopInfoQuery& query) const {
	GetStopInfo info;
	info.stop_name = move(query.stop_name);
	info.found = stops_info.count(info.stop_name);
	info.req_id = query.req_id;
	if (info.found) {
		info.buses = { stops_info.at(info.stop_name).buses.begin(), stops_info.at(info.stop_name).buses.end() };
	}
	return info;
}

void TransportGuider::ProcessBusStopsQuery(BusStopsQuery& query) {
	for (const auto& stop : query.stops) {
		stops_info[stop].buses.insert(query.bus_id);
	}
	buses_info[move(query.bus_id)] = BusInfo(move(query.stops), query.is_circled);
}

GetBusInfo TransportGuider::ProcessGetBusInfoQuery(GetBusInfoQuery& query) const {
	if (buses_info.count(query.bus_id)) {
		GetBusInfo info;
		const BusInfo& bus_info = buses_info.at(query.bus_id);
		info.bus_id = move(query.bus_id);
		info.is_circled = buses_info.at(info.bus_id).is_circled;
		info.all_stops_count = info.is_circled ? bus_info.stops.size()
			: 2 * bus_info.stops.size() - 1;
		info.unique_stops_count = UniqueStopsCount(bus_info.stops);
		info.length = info.is_circled ? GetLength(bus_info.stops) : 2 * GetLength(bus_info.stops);
		info.real_length = GetRealLength(bus_info.stops, info.is_circled);
		info.req_id = query.req_id;
		return info;
	}
	else {
		return GetBusInfo(move(query.bus_id), 0, 0, 0, 0, 0, query.req_id);
	}
}

size_t TransportGuider::UniqueStopsCount(const vector<string>& stops) const {
	unordered_set<string> u_stops;
	for (const auto& stop : stops) u_stops.insert(stop);
	return u_stops.size();
}

double TransportGuider::GetLength(const vector<string>& stops) const {
	double length = 0;
	if (stops.size() >= 2) {
		for (size_t i = 0; i < stops.size() - 1; ++i) {
			length += Length(stops_info.at(stops[i]).coords, stops_info.at(stops[i + 1]).coords);
		}
	}
	return length;
}

double TransportGuider::GetRealLength(const vector<string>& stops, bool is_circled) const {
	double length = 0;
	for (size_t i = 0; i < stops.size() - 1; ++i) {
		length += is_circled ? RealLength(stops[i], stops[i + 1]) :
			(RealLength(stops[i], stops[i + 1]) + RealLength(stops[i + 1], stops[i]));
	}
	return length;
}

void TransportGuider::InfoOutput(const Json::Document& doc, ostream& stream) const {
	Json::UploadDocument(doc, stream);
}

const unordered_map<string, StopInfo>& TransportGuider::CheckStops() const {
	return stops_info;
}

const unordered_map<string, BusInfo>& TransportGuider::CheckBuses() const {
	return buses_info;
}


Json::Node NodeFromStop(GetStopInfo info) {
	map<string, Json::Node> result;
	result["request_id"] = Json::Node(static_cast<double>(info.req_id));

	if (!info.found) {
		result["error_message"] = Json::Node(string("not found"));
	}
	else {
		vector<Json::Node> buses;
		for (string& bus : info.buses) {
			buses.push_back(Json::Node(move(bus)));
		}
		result["buses"] = Json::Node(move(buses));
	}
	return Json::Node(move(result));
}

Json::Node NodeFromBus(GetBusInfo info) {
	map<string, Json::Node> result;

	result["request_id"] = Json::Node(static_cast<double>(info.req_id));
	if (info.all_stops_count == 0) {
		result["error_message"] = Json::Node(string("not found"));
	}
	else {
		result["route_length"] = Json::Node(static_cast<double>(info.real_length));
		result["curvature"] = Json::Node(static_cast<double>(info.real_length / info.length));
		result["stop_count"] = Json::Node(static_cast<double>(info.all_stops_count));
		result["unique_stop_count"] = Json::Node(static_cast<double>(info.unique_stops_count));
	}
	return Json::Node(move(result));
}