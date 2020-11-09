#include "guider.h"
#include <algorithm>

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
			BusInfoOutput(move(info));
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
		GetBusInfo result;
		result.bus_id = move(query.bus_id);
		result.all_stops_count = bus_info.stops.size();
		result.length = GetLength(bus_info.stops);
		result.unique_stops_count = UniqueStopsCount(bus_info.stops);
	}
	else {
		result.push_back(GetBusInfo(move(query.bus_id), 0, 0, 0));
	}
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

double TransportGuider::Length(Coordinates from, Coordinates to) const {

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
