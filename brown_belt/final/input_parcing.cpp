#include "input_parsing.h"

template <typename Number>
Number ReadNumberOnLine(istream& stream) {
	Number number;
	stream >> number;
	string dummy;
	getline(stream, dummy);
	return number;
}


QueryType ConvertStrToType(const string& str) {
	if (str == "Bus") return QueryType::BUS;
	else if (str == "Stop") return QueryType::STOP;
	else throw invalid_argument("Unknown query type");
}

vector<Query> ReadQueries(istream& input) {
	vector<Query> queries;
	int query_count = ReadNumberOnLine<int>(input);
	return move(queries);
}