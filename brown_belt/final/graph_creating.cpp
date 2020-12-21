#include "guider.h"
#include "profile.h"
#include <cassert>

/* PUBLIC_METHODS---PUBLIC_METHODS---PUBLIC_METHODS---PUBLIC_METHODS---PUBLIC_METHODS */


const unordered_map<string_view, Knot>& TransportGraph::CheckKnots() const {
	return knots_;
}

bool TransportGraph::GraphExist() const {
	return graph.has_value();
}

optional<Graph::Router<double>::RouteInfo> TransportGraph::BuildRoute(size_t from, size_t to) const {
	return router_ptr->BuildRoute(from, to);
}

void TransportGraph::ReleaseRouteCache(uint64_t id) const {
	router_ptr->ReleaseRoute(id);
}

size_t TransportGraph::StopId(string_view stop) const {
	return knots_.at(stop).id;
}

void TransportGraph::Create() {
	{
		LOG_DURATION("Graph creating");
		const size_t vertex_count = AllStopsCount();
		id_to_knot.resize(vertex_count);
		graph = Graph::DirectedWeightedGraph<double>(vertex_count);
		for (const auto& bus : buses_) {
			if (bus.second.is_circled) {
				size_t from_id = NewVertex(bus.second.stops[0]);
				size_t to_id = NewVertex(bus.second.stops[1]);
				CreateBusEdge(from_id, to_id, bus.first);

				for (size_t i = 1; i < bus.second.stops.size() - 1; i++) {
					size_t from_id = LastVertex(); //vertex i
					size_t to_id = NewVertex(bus.second.stops[i + 1]);
					CreateBusEdge(from_id, to_id, bus.first);
				} //�� ���� ��������� ������� ������ ����������� � ������ ����
			}
			else {
				size_t from_id = NewVertex(bus.second.stops[0]);
				size_t to_id = NewVertex(bus.second.stops[1]);
				CreateBusEdge(to_id, from_id, bus.first);
				size_t transfer = NewVertex(bus.second.stops[0]); //������ �������� � �������� from_id
				CreateBusEdge(transfer, to_id, bus.first); //����� ������������������!!!

				for (size_t i = 1; i < bus.second.stops.size() - 1; i++) {
					size_t from_id = LastVertex(); //vertex i
					size_t to_id = NewVertex(bus.second.stops[i + 1]);
					CreateBusEdge(to_id, from_id, bus.first);
					CreateBusEdge(from_id, to_id, bus.first);
				}
			}
		}
	}
	router_ptr = make_unique<Graph::Router<double>>(graph.value());
}

pair<double, vector<ItemPtr>> TransportGraph::CreateItems(Graph::Router<double>::RouteInfo info) const {
	double total_time = 0;
	vector<Graph::EdgeId> edges;
	for (size_t i = 0; i < info.edge_count; ++i) {
		edges.push_back(router_ptr->GetRouteEdge(info.id, i));
	}
	vector<ItemPtr> result;

	if (edges_[edges[0]].type == "Bus") {
		result.push_back(make_unique<Wait>(string(id_to_knot[graph.value().GetEdge(edges[0]).from]), config_.time));
		total_time += config_.time;
	}
	if (edges_[edges.back()].type == "Wait")
		edges.resize(edges.size() - 1);

	for (auto edge_to_proc = edges.begin(); edge_to_proc < edges.end();) {
		auto& edge = edges_[*edge_to_proc];
		if (edge.type == "Wait") {
			result.push_back(make_unique<Wait>(string(edge.name), edge.time));
			total_time += config_.time;
			edge_to_proc++;
		}
		else {
			int counter = 1;
			double time = edge.time;
			for (auto it = edge_to_proc;
				it < prev(edges.end()) && edges_[*next(it)].type == "Bus" && edges_[*next(it)].name == edge.name;
				it++) {
				counter++;
				time += edges_[*next(it)].time;
				edge_to_proc++;
			}
			result.push_back(make_unique<Bus>(string(edge.name), counter, time));
			total_time += time;
			edge_to_proc++;
		}
	}

	return make_pair(total_time, move(result));
}


/* PRIVATE_METHODS---PRIVATE_METHODS---PRIVATE_METHODS---PRIVATE_METHODS---PRIVATE_METHODS */


size_t TransportGraph::AllStopsCount() const { //! ����� ����� ��������� ��������� ���������� ������!!!
	size_t counter = 0;
	for (const auto& bus : buses_) {
		counter += bus.second.stops.size() + 1;
	}
	return counter;
}

void TransportGraph::CreateWaitEdge(size_t from, size_t to) {
	graph.value().AddEdge(Graph::Edge<double>{from, to, config_.time});
	edges_.push_back({ "Wait", id_to_knot[from], config_.time });
}

double TransportGraph::Length(size_t from, size_t to) const {
	return stops_.at(string(id_to_knot[from])).distances.at(string(id_to_knot[to]));
}

void TransportGraph::CreateBusEdge(size_t from, size_t to, string_view bus_name) {
	double time = Length(from, to) / config_.velocity / 1000;
	graph.value().AddEdge(Graph::Edge<double>{from, to, time});
	edges_.push_back({ "Bus", bus_name, time });
	last_bus_edge_to_id = int(to);
}

void TransportGraph::TieToKnot(const unordered_set<size_t>& other, size_t v) {
	for (size_t another : other) {
		CreateWaitEdge(v, another);
		CreateWaitEdge(another, v);
	}
}

size_t TransportGraph::NewVertex(string_view v) {
	id_to_knot[last_id + 1] = v;
	if (!knots_.count(v)) {
		knots_[v].id = last_id + 1;
		knots_[v].vertexes.insert(last_id + 1);
	}
	else {
		TieToKnot(knots_[v].vertexes, last_id + 1);
		knots_[v].vertexes.insert(last_id + 1);
	}
	return ++last_id;
}

size_t TransportGraph::LastVertex() const {
	return last_bus_edge_to_id;
}