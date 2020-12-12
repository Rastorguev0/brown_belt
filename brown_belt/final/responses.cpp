#include "responses.h"

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
