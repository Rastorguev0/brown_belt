#include "guider.h"
#include "profile.h"
#include <cassert>

/* PUBLIC_METHODS---PUBLIC_METHODS---PUBLIC_METHODS---PUBLIC_METHODS---PUBLIC_METHODS */


bool TransportGraph::GraphExist() const {
	return graph.has_value();
}

optional<GetRouteInfo> TransportGraph::BuildRoute(Graph::VertexId from, Graph::VertexId to) const {
	auto route = router_ptr->BuildRoute(from, to);
	if (!route) {
		return nullopt;
	}
	else {
		vector<Graph::EdgeId> edges;
		for (size_t i = 0; i < route.value().edge_count; ++i) {
			edges.push_back(router_ptr->GetRouteEdge(route.value().id, i));
		}

		GetRouteInfo result;
		for (const auto edge_id : edges) {
			result.total_time += edges_[edge_id]->time;
			result.items.push_back(edges_[edge_id]);
		}
		router_ptr->ReleaseRoute(route.value().id);
		return move(result);
	}
}

Graph::VertexId TransportGraph::StopId(string_view stop) const {
	return knots_.at(stop).in;
}

void TransportGraph::Create() {
	LOG_DURATION("Graph and route creating");
	{
		const size_t vertex_count = stops_.size() * 2;
		graph = DoubleGraph(vertex_count);
		FillWithStops();
		FillWithBuses();
	}
	router_ptr = make_unique<Router>(graph.value());
}


/* PRIVATE_METHODS---PRIVATE_METHODS---PRIVATE_METHODS---PRIVATE_METHODS---PRIVATE_METHODS */


double TransportGraph::Length(size_t from, size_t to) const {
	return stops_.at(string(id_to_stop_[from])).distances.at(string(id_to_stop_[to]));
}

void TransportGraph::FillWithStops() {
	id_to_stop_.resize(stops_.size() * 2);
	Graph::VertexId last_id = 0;
	for (const auto& [stop_name, stop_info] : stops_) {
		Graph::VertexId in_id = knots_[stop_name].in = last_id;
		id_to_stop_[last_id++] = stop_name;
		Graph::VertexId out_id = knots_[stop_name].out = last_id;
		id_to_stop_[last_id++] = stop_name;

		graph.value().AddEdge(Graph::Edge<double>{in_id, out_id, config_.time});
		edges_.push_back(make_shared<Wait>(stop_name, config_.time));
	}
}

void TransportGraph::AddEdge(
	string_view name, Graph::VertexId from, Graph::VertexId to, double time, size_t spans) {
	graph.value().AddEdge(Graph::Edge<double>{from, to, time});
	edges_.push_back(make_shared<Bus>(name, static_cast<int>(spans), time));
}

void TransportGraph::FillWithBuses() {
	for (const auto& [bus_name, bus_info] : buses_) {
		const auto& stops = bus_info.stops;
		const size_t all_stops_count = stops.size();
		if (bus_info.is_circled) {
			for (size_t i = 0; i + 1 < all_stops_count; ++i) {
				const Graph::VertexId from = knots_[stops[i]].out;
				double total_length = 0;
				for (size_t j = i + 1; j < all_stops_count; ++j) {
					Graph::VertexId to = knots_[stops[j]].in;
					total_length += Length(knots_[stops[j - 1]].in, to);
					AddEdge(bus_name, from, to, total_length / 1000 / config_.velocity, j - i);
				}
			}
		}
		else {
			for (int i = 0; i < all_stops_count; ++i) {
				const Graph::VertexId from = knots_[stops[i]].out;
				double total_length = 0;
				for (int j = i - 1; j >= 0; --j) {
					Graph::VertexId to = knots_[stops[j]].in;
					total_length += Length(knots_[stops[j + 1]].in, to);
					AddEdge(bus_name, from, to, total_length / 1000 / config_.velocity, i - j);
				}
				total_length = 0;
				for (int k = i + 1; k < all_stops_count; ++k) {
					Graph::VertexId to = knots_[stops[k]].in;
					total_length += Length(knots_[stops[k - 1]].in, to);
					AddEdge(bus_name, from, to, total_length / 1000 / config_.velocity, k - i);
				}
			}
		}
	}
}