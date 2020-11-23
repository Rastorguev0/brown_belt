#pragma once
#include "test_runner.h"
#include "input_parsing.h"
#include "guider.h"
#include <fstream>
using namespace std;
/*
bool StopQueriesEqual(const StopQuery& lhs, Query& rhs) {
	StopQuery* rhs_casted = StopCast(rhs);
	return rhs_casted ? *rhs_casted == lhs : false;
}

bool GetStopInfoQueriesEqual(const GetStopInfoQuery& lhs, Query& rhs) {
	GetStopInfoQuery* rhs_casted = StopGetCast(rhs);
	return rhs_casted ? *rhs_casted == lhs : false;
}

bool BusStopsQueriesEqual(const BusStopsQuery& lhs, Query& rhs) {
	BusStopsQuery* rhs_casted = BusStopsCast(rhs);
	return rhs_casted ? *rhs_casted == lhs : false;
}

bool GetBusInfoQueriesEqual(const GetBusInfoQuery& lhs, Query& rhs) {
	GetBusInfoQuery* rhs_casted = BusGetCast(rhs);
	return rhs_casted ? *rhs_casted == lhs : false;
}

bool IsQueriesEqual(QueryPtr lhs, QueryPtr rhs) {
	if (lhs->type == QueryType::STOP && rhs->type == QueryType::STOP) {
		return *StopCast(*lhs) == *StopCast(*lhs);
	}
	else if (lhs->type == QueryType::GET_STOP_INFO && rhs->type == QueryType::GET_STOP_INFO) {
		return *StopGetCast(*lhs) == *StopGetCast(*rhs);
	}
	else if (lhs->type == QueryType::BUS_STOPS && rhs->type == QueryType::BUS_STOPS) {
		return *BusStopsCast(*lhs) == *BusStopsCast(*rhs);
	}
	else if (lhs->type == QueryType::GET_BUS_INFO && rhs->type == QueryType::GET_BUS_INFO) {
		return *BusGetCast(*lhs) == *BusGetCast(*rhs);
	}
	else throw invalid_argument("Can not cast queries");
}

void TestCheckDelimiterType() {
	{
		string line = "Bus 1121 ABC: stop 1 - stop2";
		ASSERT_EQUAL(CheckDelimiterType(line), '-');
	}
	{
		string line = "Bus 1121 ABC: stop 1 > stop2";
		ASSERT_EQUAL(CheckDelimiterType(line), '>');
	}
}

void TestGetSeparatedToken() {
	{
		string line = "Stop 1251 x: 25, 5";
		string_view linref(line);
		ASSERT_EQUAL(GetSeparatedToken(linref), "Stop");
		ASSERT_EQUAL(linref, "1251 x: 25, 5");
		ASSERT_EQUAL(GetSeparatedToken(linref, ':'), "1251 x");
		ASSERT_EQUAL(linref, "25, 5");
		ASSERT_EQUAL(GetSeparatedToken(linref, ','), "25");
		ASSERT_EQUAL(linref, "5");
		ASSERT_EQUAL(GetSeparatedToken(linref), "5");
	}
	{
		string line = "Bus 1121 ABC: stop 1 - stop2";
		string_view linref(line);
		ASSERT_EQUAL(GetSeparatedToken(linref), "Bus");
		ASSERT_EQUAL(GetSeparatedToken(linref, ':'), "1121 ABC");
		ASSERT_EQUAL(GetSeparatedToken(linref, '-'), "stop 1");
		ASSERT_EQUAL(linref, "stop2");
		ASSERT_EQUAL(GetSeparatedToken(linref), "stop2");
	}
}

void TestConvertToDouble() {
	{
		string doublya = "0.2077";
		ASSERT_EQUAL(ConvertToDouble(doublya), 0.2077);
	}
	{
		string doublya = "5";
		ASSERT_EQUAL(ConvertToDouble(doublya), 5);
	}
}

void TestConvertFromMeters() {
	{
		string s = "8400m";
		ASSERT_EQUAL(ConvertFromMeters(s), 8400);
	}
}

void TestParsing() {
	{
		string query = "Stop 1251 x: 25, 5";
		ASSERT(StopQueriesEqual(StopQuery("1251 x", 25, 5, {}), *ParsePutQuery(query)));
	}
	{
		string query = "Stop 1121M";
		ASSERT(GetStopInfoQueriesEqual(GetStopInfoQuery(string("1121M")), *ParseGetQuery(query)));
	}
	{
		string query = "Bus 1251 x: stop1 - stop 2";
		ASSERT(BusStopsQueriesEqual(BusStopsQuery("1251 x", { "stop1", "stop 2" }, false), *ParsePutQuery(query)));
	}
	{
		string query = "Bus 1251 x";
		ASSERT(GetBusInfoQueriesEqual(GetBusInfoQuery(string("1251 x")), *ParseGetQuery(query)));
	}
}

void TestReadQuerries() {
	istringstream input(R"(10
		Stop Tolstopaltsevo : 55.611087, 37.20829
		Stop Marushkino : 55.595884, 37.209755
		Bus 256 : Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
		Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
		Stop Rasskazovka : 55.632761, 37.333324
		Stop Biryulyovo Zapadnoye : 55.574371, 37.6517
		Stop Biryusinka : 55.581065, 37.64839
		Stop Universam : 55.587655, 37.645687
		Stop Biryulyovo Tovarnaya : 55.592028, 37.653656
		Stop Biryulyovo Passazhirskaya : 55.580999, 37.659164
		5
		Bus 256
		Bus 750
		Stop Rasskazovka
		Bus 751
		Stop Samara)");
	vector<QueryPtr> queries = ReadQueries(input);

	vector<QueryPtr> expected;

	expected.push_back(make_unique<StopQuery>("Tolstopaltsevo", 55.611087, 37.20829, Distances()));
	expected.push_back(make_unique<StopQuery>("Marushkino", 55.595884, 37.209755, Distances()));
	expected.push_back(make_unique<BusStopsQuery>("256", vector<string>{"Biryulyovo Zapadnoye", "Biryusinka",
		"Universam", "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya", "Biryulyovo Zapadnoye"}, true));
	expected.push_back(make_unique<BusStopsQuery>("750", vector<string>{"Tolstopaltsevo", "Marushkino", "Rasskazovka"}, false));
	expected.push_back(make_unique<StopQuery>("Rasskazovka", 55.632761, 37.333324, Distances()));
	expected.push_back(make_unique<StopQuery>("Biryulyovo Zapadnoye", 55.574371, 37.6517, Distances()));
	expected.push_back(make_unique<StopQuery>("Biryusinka", 55.581065, 37.64839, Distances()));
	expected.push_back(make_unique<StopQuery>("Universam", 55.587655, 37.645687, Distances()));
	expected.push_back(make_unique<StopQuery>("Biryulyovo Tovarnaya", 55.592028, 37.653656, Distances()));
	expected.push_back(make_unique<StopQuery>("Biryulyovo Passazhirskaya", 55.580999, 37.659164, Distances()));
	expected.push_back(make_unique<GetBusInfoQuery>(string("256")));
	expected.push_back(make_unique<GetBusInfoQuery>(string("750")));
	expected.push_back(make_unique<GetStopInfoQuery>(string("Rasskazovka")));
	expected.push_back(make_unique<GetBusInfoQuery>(string("751")));
	expected.push_back(make_unique<GetStopInfoQuery>(string("Samara")));

	for (size_t i = 0; i < queries.size(); i++) {
		ASSERT(IsQueriesEqual(move(queries[i]), move(expected[i])));
	}
}

void TestProcessStopQuery() {
	TransportGuider guider;
	StopQuery sq("stop1", 1.1, 1.2, Distances{ {"stop2", 10}, {"stop3", 20 } });
	StopQuery sq2("stop2", 0, 1, Distances());
	guider.ProcessStopQuery(sq);
	guider.ProcessStopQuery(sq2);
	ASSERT_EQUAL(guider.CheckStops().size(), 3);
	ASSERT(guider.CheckStops().count("stop1"));
	ASSERT(guider.CheckStops().count("stop2"));
	ASSERT(guider.CheckStops().count("stop3"));
	//todo REWORK THIS ASSERTS!
	ASSERT_EQUAL(guider.CheckStops().at("stop1"), StopInfo({ 1.1, 1.2 }, {}, { {"stop2", 10}, { "stop3", 20 } }));
	ASSERT_EQUAL(guider.CheckStops().at("stop2"), StopInfo({ 0, 1 }, {}, { {"stop1", 10} }));
}

void TestProcessGetStopInfoQuery() {
	TransportGuider guider;

	vector<QueryPtr> queries;
	queries.push_back(make_unique<StopQuery>("stopA", 0, 0, Distances()));
	queries.push_back(make_unique<BusStopsQuery>("bus1", vector<string>{"stopA", "stopB", "stopC"}, false));
	queries.push_back(make_unique<BusStopsQuery>("bus2", vector<string>{"stopD", "stopB", "stopC"}, false));
	queries.push_back(make_unique<StopQuery>("stopG", 0, 0, Distances()));
	queries.push_back(make_unique<StopQuery>("stopB", 0, 0, Distances()));
	queries.push_back(make_unique<StopQuery>("stopC", 0, 0, Distances()));
	queries.push_back(make_unique<StopQuery>("stopD", 0, 0, Distances()));

	guider.ProcessQueries(move(queries));

	GetStopInfoQuery gsiq1(string("stopG"));
	GetStopInfoQuery gsiq2(string("stopF"));
	GetStopInfoQuery gsiq3(string("stopA"));
	GetStopInfoQuery gsiq4(string("stopB"));

	ASSERT_EQUAL(guider.ProcessGetStopInfoQuery(gsiq1), GetStopInfo("stopG", vector<string>{}, true));
	ASSERT_EQUAL(guider.ProcessGetStopInfoQuery(gsiq2), GetStopInfo("stopF", vector<string>{}, false));
	ASSERT_EQUAL(guider.ProcessGetStopInfoQuery(gsiq3), GetStopInfo("stopA", vector<string>{"bus1"}, true));
	ASSERT_EQUAL(guider.ProcessGetStopInfoQuery(gsiq4), GetStopInfo("stopB", vector<string>{"bus1", "bus2"}, true));
}

void TestProcessBusStopsQuery() {
	TransportGuider guider;
	BusStopsQuery q1("bus 1", vector<string>{"s1", "s 2", "s3"}, false);
	BusStopsQuery q2("bus 2", vector<string>{"s1", "s 2", "s4", "s1"}, true);
	guider.ProcessBusStopsQuery(q1);
	guider.ProcessBusStopsQuery(q2);
	ASSERT_EQUAL(guider.CheckBuses().size(), 2);
	ASSERT(guider.CheckBuses().count("bus 1"));
	ASSERT(guider.CheckBuses().count("bus 2"));
	ASSERT_EQUAL(guider.CheckBuses().at("bus 1"), BusInfo({ vector<string>{"s1", "s 2", "s3"}, false }));
	ASSERT_EQUAL(guider.CheckBuses().at("bus 2"), BusInfo({ vector<string>{"s1", "s 2", "s4", "s1"}, true }));
}

void TestProcessGetBusInfoQuery() {
	TransportGuider guider;

	StopQuery sq("stop1", 0, 0, Distances{ { "stop2", 10 } });
	StopQuery sq2("stop2", 0, 1, Distances());
	guider.ProcessStopQuery(sq);
	guider.ProcessStopQuery(sq2);

	BusStopsQuery q1("bus 1", vector<string>{"stop1"}, false);
	BusStopsQuery q2("bus 2", vector<string>{"stop1", "stop2", "stop1"}, true);
	guider.ProcessBusStopsQuery(q1);
	guider.ProcessBusStopsQuery(q2);

	auto gbiq1 = GetBusInfoQuery(string("bus 1"));
	auto gbiq2 = GetBusInfoQuery(string("bus 2"));
	GetBusInfo gbi1 = guider.ProcessGetBusInfoQuery(gbiq1);
	GetBusInfo gbi2 = guider.ProcessGetBusInfoQuery(gbiq2);

	ASSERT_EQUAL(gbi1, GetBusInfo("bus 1", 1, 1, 0, 0, false));
	ASSERT_EQUAL(gbi2, GetBusInfo("bus 2", 3, 2, 2 * Length(sq.coords, sq2.coords), 20, true));
}

void MainTest() {
	stringstream out("");
	TransportGuider guider;

	vector<QueryPtr> queries;
	queries.push_back(make_unique<StopQuery>("Tolstopaltsevo", 55.611087, 37.20829, Distances()));
	queries.push_back(make_unique<StopQuery>("Marushkino", 55.595884, 37.209755, Distances()));
	queries.push_back(make_unique<BusStopsQuery>("256", vector<string>{"Biryulyovo Zapadnoye", "Biryusinka",
		"Universam", "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya", "Biryulyovo Zapadnoye"}, true));
	queries.push_back(make_unique<BusStopsQuery>("750", vector<string>{"Tolstopaltsevo", "Marushkino", "Rasskazovka"}, false));
	queries.push_back(make_unique<StopQuery>("Rasskazovka", 55.632761, 37.333324, Distances()));
	queries.push_back(make_unique<StopQuery>("Biryulyovo Zapadnoye", 55.574371, 37.6517, Distances()));
	queries.push_back(make_unique<StopQuery>("Biryusinka", 55.581065, 37.64839, Distances()));
	queries.push_back(make_unique<StopQuery>("Universam", 55.587655, 37.645687, Distances()));
	queries.push_back(make_unique<StopQuery>("Biryulyovo Tovarnaya", 55.592028, 37.653656, Distances()));
	queries.push_back(make_unique<StopQuery>("Biryulyovo Passazhirskaya", 55.580999, 37.659164, Distances()));
	queries.push_back(make_unique<GetBusInfoQuery>(string("256")));
	queries.push_back(make_unique<GetBusInfoQuery>(string("750")));
	queries.push_back(make_unique<GetBusInfoQuery>(string("751")));

	guider.ProcessQueries(move(queries), out);
	string expected = R"(Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length
Bus 750: 5 stops on route, 3 unique stops, 20939.5 route length
Bus 751: not found
)";
	ASSERT_EQUAL(out.str(), expected);
}

void MainTestV2() {
	/*
	stringstream out("");
	TransportGuider guider;

	vector<QueryPtr> queries;
	queries.push_back(make_unique<StopQuery>("Tolstopaltsevo", 55.611087, 37.20829, Distances()));
	queries.push_back(make_unique<StopQuery>("Marushkino", 55.595884, 37.209755, Distances()));
	queries.push_back(make_unique<BusStopsQuery>("256", vector<string>{"Biryulyovo Zapadnoye", "Biryusinka",
		"Universam", "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya", "Biryulyovo Zapadnoye"}, true));
	queries.push_back(make_unique<BusStopsQuery>("750", vector<string>{"Tolstopaltsevo", "Marushkino", "Rasskazovka"}, false));
	queries.push_back(make_unique<StopQuery>("Rasskazovka", 55.632761, 37.333324, Distances()));
	queries.push_back(make_unique<StopQuery>("Biryulyovo Zapadnoye", 55.574371, 37.6517, Distances()));
	queries.push_back(make_unique<StopQuery>("Biryusinka", 55.581065, 37.64839, Distances()));
	queries.push_back(make_unique<StopQuery>("Universam", 55.587655, 37.645687, Distances()));
	queries.push_back(make_unique<StopQuery>("Biryulyovo Tovarnaya", 55.592028, 37.653656, Distances()));
	queries.push_back(make_unique<StopQuery>("Biryulyovo Passazhirskaya", 55.580999, 37.659164, Distances()));
	queries.push_back(make_unique<BusStopsQuery>("828", vector<string>{"Biryulyovo Zapadnoye", "Universam",
	"Rossoshanskaya ulitsa", "Biryulyovo Zapadnoye"}, true));
	queries.push_back(make_unique<StopQuery>("Rossoshanskaya ulitsa", 55.595579, 37.605757, Distances()));
	queries.push_back(make_unique<StopQuery>("Prazhskaya", 55.611678, 37.603831, Distances()));
	queries.push_back(make_unique<GetBusInfoQuery>(string("256")));
	queries.push_back(make_unique<GetBusInfoQuery>(string("750")));
	queries.push_back(make_unique<GetBusInfoQuery>(string("751")));
	queries.push_back(make_unique<GetStopInfoQuery>(string("Samara")));
	queries.push_back(make_unique<GetStopInfoQuery>(string("Prazhskaya")));
	queries.push_back(make_unique<GetStopInfoQuery>(string("Biryulyovo Zapadnoye")));

	guider.ProcessQueries(move(queries), out);
	string expected = R"(Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length
Bus 750: 5 stops on route, 3 unique stops, 20939.5 route length
Bus 751: not found
Stop Samara: not found
Stop Prazhskaya: no buses
Stop Biryulyovo Zapadnoye: buses 256 828
)";
	ASSERT_EQUAL(out.str(), expected);

}

void MainTestV3() {
	stringstream out;
	istringstream in(R"(13
Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino
Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
Stop Rasskazovka: 55.632761, 37.333324
Stop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam
Stop Biryusinka: 55.581065, 37.64839, 750m to Universam
Stop Universam: 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656, 1300m to Biryulyovo Passazhirskaya
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164, 1200m to Biryulyovo Zapadnoye
Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye
Stop Rossoshanskaya ulitsa: 55.595579, 37.605757
Stop Prazhskaya: 55.611678, 37.603831
6
Bus 256
Bus 750
Bus 751
Stop Samara
Stop Prazhskaya
Stop Biryulyovo Zapadnoye
)");
	TransportGuider guider;
	guider.ProcessQueries(ReadQueries(in), out);
	
	string expected(R"(Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.361239 curvature
Bus 750: 5 stops on route, 3 unique stops, 27600 route length, 1.318084 curvature
Bus 751: not found
Stop Samara: not found
Stop Prazhskaya: no buses
Stop Biryulyovo Zapadnoye: buses 256 828
)");
	ASSERT_EQUAL(out.str(), expected);
}

*/
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
"unique_stop_count": 5,
"route_length": 5950,
"stop_count": 6
},
{
"request_id": 519139350,
"curvature": 1.31808,
"route_length": 27600,
"unique_stop_count": 3,
"stop_count": 5
},
{
"request_id": 194217464,
"error_message": "not found"
},
{
"request_id": 746888088,
"error_message": "not found"
},
{
"request_id": 65100610,
"buses": []
},
{
"request_id": 1042838872,
"buses": [
"256",
"828"
]
}
])");
		ASSERT_EQUAL(out.str(), expected);
	}
}

void TestAll() {
	TestRunner tr;
	/*
	RUN_TEST(tr, TestCheckDelimiterType);
	RUN_TEST(tr, TestGetSeparatedToken);
	RUN_TEST(tr, TestConvertToDouble);
	RUN_TEST(tr, TestConvertFromMeters);
	RUN_TEST(tr, TestParsing);
	RUN_TEST(tr, TestReadQuerries);
	RUN_TEST(tr, TestProcessStopQuery);
	RUN_TEST(tr, TestProcessGetStopInfoQuery);
	RUN_TEST(tr, TestProcessBusStopsQuery);
	RUN_TEST(tr, TestProcessGetBusInfoQuery);
	RUN_TEST(tr, MainTest);
	RUN_TEST(tr, MainTestV2);
	RUN_TEST(tr, MainTestV3);
	*/
	RUN_TEST(tr, MainTestV4);
}