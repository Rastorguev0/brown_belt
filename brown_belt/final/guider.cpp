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

ostream& operator << (ostream& os, const GetStopInfo& info) {
	os << "Stop " << info.stop_name << ": ";
	if (!info.found) os << "not found" << endl;
	else {
		if (info.buses.size() == 0)	os << "no buses" << endl;
		else {
			os << "buses";
			for (const auto& bus : info.buses) {
				os << " " << bus;
			}
			os << endl;
		}
	}
	return os;
}

ostream& operator<< (ostream& os, const GetBusInfo& info) {
	os << "Bus " << info.bus_id << ": ";
	if (info.all_stops_count == 0) os << "not found" << endl;
	else {
		double curv = info.real_length / info.length;
		os << info.all_stops_count << " stops on route, "
			<< info.unique_stops_count << " unique stops, "
			<< info.real_length << " route length, "
			<< std::setprecision(7) << curv << " curvature" << endl;
	}
	return os;
}

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
	for (const auto& query : queries) {
		switch (query->type) {
		case QueryType::STOP:
			ProcessStopQuery(*StopCast(*query));
			break;
		case QueryType::GET_STOP_INFO:
			InfoOutput(ProcessGetStopInfoQuery(*StopGetCast(*query)), stream);
			break;
		case QueryType::BUS_STOPS:
			ProcessBusStopsQuery(*BusStopsCast(*query));
			break;
		case QueryType::GET_BUS_INFO:
			InfoOutput(ProcessGetBusInfoQuery(*BusGetCast(*query)), stream);
			break;
		}
	}
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
		return info;
	}
	else {
		return GetBusInfo(move(query.bus_id), 0, 0, 0, 0, 0);
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
			(RealLength(stops[i], stops[i + 1]) + RealLength(stops[i+1], stops[i]));
	}
	return length;
}

template<typename Info>
void TransportGuider::InfoOutput(const Info& info, ostream& stream) const {
	stream << info;
}

const unordered_map<string, StopInfo>& TransportGuider::CheckStops() const {
	return stops_info;
}

const unordered_map<string, BusInfo>& TransportGuider::CheckBuses() const {
	return buses_info;
}
