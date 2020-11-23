#include "input_parsing.h"

double Coordinates::LatRad() const {
	return latitude * 3.1415926535 / 180;
}
double Coordinates::LongRad() const {
	return longitude * 3.1415926535 / 180;
}

/*
ostream& operator<<(ostream& os, const Coordinates& c) {
	return os << c.latitude << " " << c.longitude;
}

const char CheckDelimiterType(string_view line) {
	auto del_it = find_if(begin(line), end(line), [](const char ch) {
		for (const auto delimeter : DELIMETERS) {
			if (ch == delimeter) return true;
		}
		return false;
		});
	if (del_it != end(line)) return *del_it;
	else throw invalid_argument("No one delimeters found");
}

string GetSeparatedToken(string_view& line, const char delimeter) {
	auto del_pos = line.find(delimeter);
	if (del_pos == string_view::npos) {
		string token = string(line);
		line.remove_prefix(line.size());
		return token;
	}
	else {
		string token = string(line.substr(0, del_pos));
		string_view token_view(token);
		line.remove_prefix(del_pos + 1);
		Trim(line);
		Trim(token_view);
		return string(token_view);
	}
}

template <typename Number>
Number ReadNumberOnLine(istream& stream) {
	Number number;
	stream >> number;
	string dummy;
	getline(stream, dummy);
	return number;
}

void Trim(string_view& view) {
	while (view.size() != 0 && (view[0] == ' ' || view[0] == '\t')) view.remove_prefix(1);
	while (view.size() != 0 && (view[view.size() - 1] == ' ' || view[view.size() - 1] == '\t')) view.remove_suffix(1);
}

double ConvertToDouble(string_view line) {
	string doublya = string(line);
	istringstream converter(move(doublya));
	double result;
	converter >> result;
	return result;
}

unsigned ConvertFromMeters(string_view line) {
	string uns = string(line.substr(0, line.size() - 1));
	istringstream converter(move(uns));
	unsigned result;
	converter >> result;
	return result;
}
*/

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