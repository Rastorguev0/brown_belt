#include "input_parsing.h"

double Coordinates::LatRad() const {
	return latitude * 3.1415926535 / 180;
}
double Coordinates::LongRad() const {
	return longitude * 3.1415926535 / 180;
}

bool Coordinates::operator==(const Coordinates& other) const {
	return make_tuple(latitude, longitude)
		== make_tuple(other.latitude, other.longitude);
}

ostream& operator<<(ostream& os, const Coordinates& c) {
	return os << c.latitude << " " << c.longitude;
}

StopQuery::StopQuery(string_view line) {
	type = QueryType::STOP;

	stop_name = GetSeparatedToken(line, ':');
	coords = {
		ConvertToDouble(GetSeparatedToken(line, ',')),
		ConvertToDouble(GetSeparatedToken(line, ',')),
	};
	while (!line.empty()) {
		unsigned dist = ConvertFromMeters(GetSeparatedToken(line));
		//"to"
		GetSeparatedToken(line);
		string stop = GetSeparatedToken(line, ',');
		if (stop != "") distances[stop] = dist;
	}
}

GetStopInfoQuery::GetStopInfoQuery(string_view line) {
	type = QueryType::GET_STOP_INFO;
	stop_name = string(line);
}

BusStopsQuery::BusStopsQuery(string_view line) {
	type = QueryType::BUS_STOPS;

	bus_id = GetSeparatedToken(line, ':');
	const char delimeter = CheckDelimiterType(line);
	is_circled = delimeter == '>';
	while (!line.empty()) {
		stops.push_back(GetSeparatedToken(line, delimeter));
	}
}

GetBusInfoQuery::GetBusInfoQuery(string_view line) {
	type = QueryType::GET_BUS_INFO;
	bus_id = string(line);
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

QueryPtr ParsePutQuery(string_view line) {
	string command = GetSeparatedToken(line);
	if (command == "Stop") return make_unique<StopQuery>(line);
	else if (command == "Bus") return make_unique<BusStopsQuery>(line);
	else throw invalid_argument("Unknown query command");
}

QueryPtr ParseGetQuery(string_view line) {
	string cmd = GetSeparatedToken(line);
	if (cmd == "Bus") return make_unique<GetBusInfoQuery>(line);
	else if (cmd == "Stop") return make_unique<GetStopInfoQuery>(line);
	else throw invalid_argument("Unknown query command");
}

vector<QueryPtr> ReadQueries(istream& input) {
	vector<QueryPtr> queries;
	int put_query_count = ReadNumberOnLine<int>(input);
	queries.reserve(put_query_count);

	for (int i = 0; i < put_query_count; i++) {
		string line;
		getline(input, line);
		queries.push_back(ParsePutQuery(line));
	}

	int get_query_count = ReadNumberOnLine<int>(input);
	queries.reserve(put_query_count + get_query_count);

	for (int i = 0; i < get_query_count; i++) {
		string line;
		getline(input, line);
		queries.push_back(ParseGetQuery(line));
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