#include "input_parsing.h"

template <typename Number>
Number ReadNumberOnLine(istream& stream) {
	Number number;
	stream >> number;
	string dummy;
	getline(stream, dummy);
	return number;
}

// ? istream or string_view? string_view I think
QueryPtr ParseQuery(string_view line) {
	
	//else throw invalid_argument("Unknown query command");
}

vector<QueryPtr> ReadQueries(istream& input) {
	vector<QueryPtr> queries;
	int query_count = ReadNumberOnLine<int>(input);
	queries.reserve(query_count);

	for (int i = 0; i < query_count; i++) {
		string line;
		getline(input, line);
		queries.push_back(ParseQuery(line));
	}

	return move(queries);
}