#pragma once
#include "test_runner.h"
#include "input_parsing.h"
#include "guider.h"
#include <fstream>
using namespace std;

void MainTestV4() {
	ifstream input("final\\input.txt");
	if (input.is_open()) {
		vector<QueryPtr> queries = ReadQueries(input);
		TransportGuider guider;

		ostringstream out;
		guider.ProcessQueries(move(queries), out);
		
		string expected(R"([
{
"curvature": 1.36124,
"request_id": 1965312327,
"route_length": 5950,
"stop_count": 6,
"unique_stop_count": 5
},
{
"curvature": 1.31808,
"request_id": 519139350,
"route_length": 27600,
"stop_count": 5,
"unique_stop_count": 3
},
{
"error_message": "not found",
"request_id": 194217464
},
{
"error_message": "not found",
"request_id": 746888088
},
{
"buses": [],
"request_id": 65100610
},
{
"buses": [
"256",
"828"
],
"request_id": 1042838872
}
])");
		ASSERT_EQUAL(out.str(), expected);
	}
}

void TestAll() {
	TestRunner tr;
	RUN_TEST(tr, MainTestV4);
}