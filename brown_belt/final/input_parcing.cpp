#include "input_parsing.h"

double Coordinates::LatRad() const {
	return latitude * 3.1415926535 / 180;
}
double Coordinates::LongRad() const {
	return longitude * 3.1415926535 / 180;
}

QueryPtr ParseGetQuery(const Json::Node& query) {
	auto map = query.AsMap();
	if (map.at("type").AsString() == "Stop") {
		return make_unique<GetStopInfoQuery>(
			map.at("name").AsString(),
			static_cast<int>(map.at("id").AsDouble())
			);
	}
	else {
		return make_unique<GetBusInfoQuery>(
			map.at("name").AsString(),
			static_cast<int>(map.at("id").AsDouble())
			);
	}
}

QueryPtr ParsePutQuery(const Json::Node& query) {
	auto map = query.AsMap();

	if (map.at("type").AsString() == "Stop") {

		Distances distances;
		for (const auto& dist : map.at("road_distances").AsMap()) {
			distances.insert({ dist.first, dist.second.AsDouble() });
		}

		return make_unique<StopQuery>(
			map.at("name").AsString(),
			map.at("latitude").AsDouble(),
			map.at("longitude").AsDouble(),
			move(distances)
			);
	}
	else {

		vector<string> stops;
		stops.reserve(map.at("stops").AsArray().size());
		for (const auto& stop : map.at("stops").AsArray()) {
			stops.push_back(stop.AsString());
		}
		return make_unique<BusStopsQuery>(
			map.at("name").AsString(),
			move(stops),
			map.at("is_roundtrip").AsBool()
			);
	}
}

vector<QueryPtr> ReadQueries(istream& input) {
	vector<QueryPtr> queries;
	
	Json::Document doc = Json::Load(input);
	auto base_requests = doc.GetRoot().AsMap().at("base_requests");
	auto stat_requests = doc.GetRoot().AsMap().at("stat_requests");

	for (const auto& req : base_requests.AsArray()) {
		queries.push_back(ParsePutQuery(req));
	}
	for (const auto& req : stat_requests.AsArray()) {
		queries.push_back(ParseGetQuery(req));
	}
	return queries;
}

StopQuery* StopCast(Query& query) {
	return dynamic_cast<StopQuery*>(&query);
}

GetStopInfoQuery* StopGetCast(Query& query) {
	return dynamic_cast<GetStopInfoQuery*>(&query);
}

BusStopsQuery* BusStopsCast(Query& query) {
	return dynamic_cast<BusStopsQuery*>(&query);
}

GetBusInfoQuery* BusGetCast(Query& query) {
	return dynamic_cast<GetBusInfoQuery*>(&query);
}