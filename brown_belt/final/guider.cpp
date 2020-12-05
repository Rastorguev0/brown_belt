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
		case QueryType::SETTINGS:
			SetConfig(*SetCast(*query));
			break;
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
		case QueryType::ROUTE:
			if (!graph) {
				CreateGraph();
				router_ptr = make_unique<Graph::Router<EdgeTime>>(graph.value());
			}
			nodes.push_back(NodeFromRoute(ProcessGetRouteInfoQuery(*RouteCast(*query))));
			break;
		}
	}
	InfoOutput(Json::Document(Json::Node(move(nodes))), stream);
}

void TransportGuider::SetConfig(SettingsQuery& query) {
	cfg.time = query.w_time;
	cfg.velocity = query.b_vel;
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

const Graph::Edge<TransportGuider::EdgeTime>& TransportGuider::GetEdge(vector<Graph::EdgeId>::iterator it) const {
	return graph.value().GetEdge(*it);
}

pair<double, vector<ItemPtr>> TransportGuider::CreateItems(Graph::Router<EdgeTime>::RouteInfo info) const {
	double total_time = 0;
	vector<Graph::EdgeId> edges;
	for (size_t i = 0; i < info.edge_count; ++i) {
		edges.push_back(router_ptr->GetRouteEdge(info.id, i));
	}
	vector<ItemPtr> result;
	auto it_begin = edges.begin();
	for (const auto it_end = edges.end(); it_begin != it_end;) {
		auto& edge = GetEdge(it_begin);
		result.push_back(make_unique<Wait>(string(id_stops[edge.from]), cfg.time));
		total_time += cfg.time;

		auto cur_bus = edge.weight.cur_bus;
		int counter = 0;
		double time = 0;
		for (; counter == 0 || (it_begin < prev(it_end)
			&& GetEdge(it_begin).weight.cur_bus == GetEdge(next(it_begin)).weight.cur_bus);
			it_begin++) {
			counter++;
			time += GetEdge(it_begin).weight.time;
		}
		result.push_back(make_unique<Bus>(string(cur_bus), counter, time));
		total_time += time;
	}

	return make_pair(total_time, move(result));
}

GetRouteInfo TransportGuider::ProcessGetRouteInfoQuery(RouteQuery& query) const {
	GetRouteInfo result;
	if (query.from == query.to) return { query.req_id, 0, {}, true };
	result.req_id = query.req_id;
	auto route = router_ptr->BuildRoute(stops_id.at(query.from), stops_id.at(query.to));
	if (route) {
		result.found = true;
		auto [time, items] = CreateItems(route.value());
		result.total_time = time;
		result.items = move(items);
	}
	router_ptr->ReleaseRoute(route.value().id);
	return result;
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
	using Json::Node;
	map<string, Node> result;
	result["request_id"] = Node(static_cast<double>(info.req_id));

	if (!info.found) {
		result["error_message"] = Node(string("not found"));
	}
	else {
		vector<Node> buses;
		for (string& bus : info.buses) {
			buses.push_back(Node(move(bus)));
		}
		result["buses"] = Node(move(buses));
	}
	return Node(move(result));
}

Json::Node NodeFromBus(GetBusInfo info) {
	using Json::Node;
	map<string, Node> result;

	result["request_id"] = Node(static_cast<double>(info.req_id));
	if (info.all_stops_count == 0) {
		result["error_message"] = Node(string("not found"));
	}
	else {
		result["route_length"] = Node(static_cast<double>(info.real_length));
		result["curvature"] = Node(static_cast<double>(info.real_length / info.length));
		result["stop_count"] = Node(static_cast<double>(info.all_stops_count));
		result["unique_stop_count"] = Node(static_cast<double>(info.unique_stops_count));
	}
	return Json::Node(move(result));
}

Json::Node NodeFromItem(ItemPtr item) {
	using Json::Node;
	map<string, Node> result;
	result["type"] = Node(item->type);
	result["time"] = Node(item->time);
	if (item->type == "Wait") {
		result["stop_name"] = Node(move(static_cast<Wait*>(item.get())->name));
	}
	else if (item->type == "Bus") {
		result["bus"] = Node(move(static_cast<Bus*>(item.get())->bus));
		result["span_count"] = Node(static_cast<double>(static_cast<Bus*>(item.get())->spans));
	}
	else throw invalid_argument("Unknown item type");
	return result;
}

Json::Node NodeFromRoute(GetRouteInfo info) {
	using Json::Node;
	map<string, Node> result;

	result["request_id"] = Node(static_cast<double>(info.req_id));
	if (info.found) {
		result["total_time"] = Node(info.total_time);

		vector<Node> items;
		for (auto&& item : info.items) {
			items.push_back(NodeFromItem(move(item)));
		}
		result["items"] = Node(move(items));
	}
	else {
		result["error_message"] = Node(string("not found"));
	}
	return Node(move(result));
}

size_t TransportGuider::GetId(string_view stop) {
	if (stops_id.count(stop)) return stops_id[stop];
	else {
		size_t result = stops_id.size();
		stops_id[stop] = result;
		assert(result == stops_id.size() - 1);
		id_stops.push_back(stop);
		assert(id_stops[stops_id[stop]] == stop);
		return result;
	}
}

Graph::Edge<TransportGuider::EdgeTime> TransportGuider::CreateEdge(
	const pair<const string, BusInfo>& bus_pair, size_t from_idx, size_t to_idx, bool transfer
) {
	auto& bus = bus_pair.second;
	size_t from = GetId(bus.stops[from_idx]);
	size_t to = GetId(bus.stops[to_idx]);

	return Graph::Edge<TransportGuider::EdgeTime>{
		from, to,
		EdgeTime{ bus_pair.first,
			RealLength(bus.stops[from_idx], bus.stops[to_idx]) / cfg.velocity / 1000,
			cfg.time,
			transfer }
	};
}

void TransportGuider::CreateGraph() {
	graph = Graph::DirectedWeightedGraph<EdgeTime>(stops_info.size());
	stops_id.reserve(stops_info.size());

	for (const auto& bus_pair : buses_info) {
		auto& bus = bus_pair.second;
		if (bus.is_circled) {
			graph.value().AddEdge(CreateEdge(bus_pair, 0, 1, true));
			for (size_t i = 1; i < bus.stops.size() - 1; ++i) {
				graph.value().AddEdge(CreateEdge(bus_pair, i, i + 1));
			}
		}
		else {
			for (size_t i = 0; i < bus.stops.size() - 1; ++i) {
				graph.value().AddEdge(CreateEdge(bus_pair, i, i + 1));
				graph.value().AddEdge(CreateEdge(bus_pair, i + 1, i));
			}
		}
	}

}

bool operator > (const TransportGuider::EdgeTime& lhs, const TransportGuider::EdgeTime& rhs) {
	return lhs.time > rhs.time + rhs.wait_time;
}

bool operator >= (const TransportGuider::EdgeTime& lhs, const TransportGuider::EdgeTime& rhs) {
	return lhs.time >= rhs.time + rhs.wait_time;
}
	
bool operator < (const TransportGuider::EdgeTime& lhs, const TransportGuider::EdgeTime& rhs) {
	return lhs.time < rhs.time - rhs.wait_time;
}

TransportGuider::EdgeTime operator + (const TransportGuider::EdgeTime& lhs, const TransportGuider::EdgeTime& rhs) {
	TransportGuider::EdgeTime result;
	result.cur_bus = rhs.cur_bus;
	result.from_is_ending = rhs.from_is_ending;
	result.time = lhs.cur_bus != rhs.cur_bus || rhs.from_is_ending ?
		lhs.time + rhs.time : lhs.time + rhs.time + lhs.wait_time;
	return result;
}