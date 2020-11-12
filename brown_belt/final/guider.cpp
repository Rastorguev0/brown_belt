#include "guider.h"
#include <algorithm>
#include <cmath>

const unsigned R = 6371000;

ostream& operator<<(ostream& os, const BusInfo& bi) {
	os << "{ ";
	for (const auto& stop : bi.stops) {
		os << stop << " ";
	}
	return os << " } " << ", " << bi.is_circled << endl;
}

ostream& operator<<(ostream& os, const StopInfo& si) {
	return os << si.coords;
}

double Length(const Coordinates& lhs, const Coordinates& rhs) {
	return R * acos(
		sin(lhs.LatRad()) * sin(rhs.LatRad()) +
		cos(lhs.LatRad()) * cos(rhs.LatRad()) *
		cos(abs(lhs.LongRad() - rhs.LongRad()))
	);
}

void TransportGuider::ProcessQueries(vector<QueryPtr> queries, ostream& stream) {
	for (const auto& query : queries) {
		switch (query->type) {
		case QueryType::STOP:
			ProcessStopQuery(*StopCast(*query));
			break;
		case QueryType::BUS_STOPS:
			ProcessBusStopsQuery(*BusStopsCast(*query));
			break;
		case QueryType::GET_BUS_INFO:
			vector<GetBusInfo> info = ProcessGetBusInfoQuery(*BusGetCast(*query));
			BusInfoOutput(move(info), stream);
			break;
		}
	}
}

void TransportGuider::ProcessStopQuery(StopQuery& query) {
	stops_info[query.stop_name] = StopInfo(query.coords);
}

void TransportGuider::ProcessBusStopsQuery(BusStopsQuery& query) {
	buses_info[query.bus_id] = BusInfo(move(query.stops), query.is_circled);
}

vector<GetBusInfo> TransportGuider::ProcessGetBusInfoQuery(GetBusInfoQuery& query) {
	vector<GetBusInfo> result;
	if (buses_info.count(query.bus_id)) {
		BusInfo& bus_info = buses_info[query.bus_id];
		GetBusInfo info;
		info.bus_id = move(query.bus_id);
		info.all_stops_count = bus_info.stops.size();
		info.length = GetLength(bus_info.stops);
		info.unique_stops_count = UniqueStopsCount(bus_info.stops);
		result.push_back(move(info));
	}
	else {
		result.push_back(GetBusInfo(move(query.bus_id), 0, 0, 0));
	}
	return move(result);
}

size_t TransportGuider::UniqueStopsCount(vector<string>& stops) const {
	unordered_set<string> u_stops;
	for (const auto& stop : stops) u_stops.insert(stop);
	return u_stops.size();
}

double TransportGuider::GetLength(vector<string>& stops) const {
	double length = 0;
	if (stops.size() >= 2) {
		for (size_t i = 0; i < stops.size() - 1; ++i) {
			length += Length(stops_info.at(stops[i]).coords, stops_info.at(stops[i + 1]).coords);
		}
	}
	return length;
}

void TransportGuider::BusInfoOutput(const vector<GetBusInfo> buses_info, ostream& stream) {
	for (const auto& info : buses_info) {
		stream << "Bus " << info.bus_id << ": ";
		if (info.all_stops_count == 0) stream << "not found" << endl;
		else {
			stream << info.all_stops_count << " stops on route, "
				<< info.unique_stops_count << " uique stops, "
				<< info.length << " route length" << endl;
		}
	}
}

const unordered_map<string, StopInfo>& TransportGuider::CheckStops() const {
	return stops_info;
}

const unordered_map<string, BusInfo>& TransportGuider::CheckBuses() const {
	return buses_info;
}
