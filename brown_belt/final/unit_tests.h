#pragma once
#include "test_runner.h"
#include "input_parsing.h"
#include "guider.h"
using namespace std;

bool StopQueriesEqual(const StopQuery& lhs, Query& rhs) {
	StopQuery* rhs_casted = StopCast(rhs);
	return rhs_casted ? *rhs_casted == lhs : false;
}

bool BusStopsQueryEqual(const BusStopsQuery& lhs, Query& rhs) {
	BusStopsQuery* rhs_casted = BusStopsCast(rhs);
	return rhs_casted ? *rhs_casted == lhs : false;
}

bool GetBusInfoQueryEqual(const GetBusInfoQuery& lhs, Query& rhs) {
	GetBusInfoQuery* rhs_casted = BusGetCast(rhs);
	return rhs_casted ? *rhs_casted == lhs : false;
}

bool IsQueriesEqual(QueryPtr lhs, QueryPtr rhs) {
	if (lhs->type == QueryType::STOP && rhs->type == QueryType::STOP) {
		return *StopCast(*lhs) == *StopCast(*lhs);
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

void TestParsing() {
	{
		string query = "Stop 1251 x: 25, 5";
		ASSERT(StopQueriesEqual(StopQuery("1251 x", 25, 5), *ParsePutQuery(query)));
	}
	{
		string query = "Bus 1251 x: stop1 - stop 2";
		ASSERT(BusStopsQueryEqual(BusStopsQuery("1251 x", { "stop1", "stop 2" }, false), *ParsePutQuery(query)));
	}
	{
		string query = "Bus 1251 x";
		ASSERT(GetBusInfoQueryEqual(GetBusInfoQuery(string("1251 x")), *ParseGetQuery(query)));
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
		3
		Bus 256
		Bus 750
		Bus 751)");
	vector<QueryPtr> queries = ReadQueries(input);

	vector<QueryPtr> expected;

	expected.push_back(make_unique<StopQuery>("Tolstopaltsevo", 55.611087, 37.20829));
	expected.push_back(make_unique<StopQuery>("Marushkino", 55.595884, 37.209755));
	expected.push_back(make_unique<BusStopsQuery>("256", vector<string>{"Biryulyovo Zapadnoye", "Biryusinka",
		"Universam", "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya", "Biryulyovo Zapadnoye"}, true));
	expected.push_back(make_unique<BusStopsQuery>("750", vector<string>{"Tolstopaltsevo", "Marushkino", "Rasskazovka"}, false));
	expected.push_back(make_unique<StopQuery>("Rasskazovka", 55.632761, 37.333324));
	expected.push_back(make_unique<StopQuery>("Biryulyovo Zapadnoye", 55.574371, 37.6517));
	expected.push_back(make_unique<StopQuery>("Biryusinka", 55.581065, 37.64839));
	expected.push_back(make_unique<StopQuery>("Universam", 55.587655, 37.645687));
	expected.push_back(make_unique<StopQuery>("Biryulyovo Tovarnaya", 55.592028, 37.653656));
	expected.push_back(make_unique<StopQuery>("Biryulyovo Passazhirskaya", 55.580999, 37.659164));
	expected.push_back(make_unique<GetBusInfoQuery>(string("256")));
	expected.push_back(make_unique<GetBusInfoQuery>(string("750")));
	expected.push_back(make_unique<GetBusInfoQuery>(string("751")));

	for (size_t i = 0; i < queries.size(); i++) {
		ASSERT(IsQueriesEqual(move(queries[i]), move(expected[i])));
	}
}

void TestProcessStopQuery() {
	TransportGuider guider;
	StopQuery sq("stop1", 0, 0);
	StopQuery sq2("stop2", 0, 1);
	guider.ProcessStopQuery(sq);
	guider.ProcessStopQuery(sq2);
	ASSERT_EQUAL(guider.CheckStops().size(), 2);
	ASSERT(guider.CheckStops().count("stop1"));
	ASSERT(guider.CheckStops().count("stop2"));
	ASSERT_EQUAL(guider.CheckStops().at("stop1"), StopInfo({ 0, 0 }));
	ASSERT_EQUAL(guider.CheckStops().at("stop2"), StopInfo({ 0, 1 }));
}

void TestProcessBusStopsQuery() {
	TransportGuider guider;
	BusStopsQuery q1("bus 1", vector<string>{"s1", "s 2", "s3"}, false);
	BusStopsQuery q2("bus 2", vector<string>{"s1", "s 2", "s4", "s1"}, true);
	ASSERT_EQUAL(guider.CheckBuses().size(), 2);
	ASSERT(guider.CheckBuses().count("bus 1"));
	ASSERT(guider.CheckBuses().count("bus 2"));
	ASSERT_EQUAL(guider.CheckBuses().at("bus 1"), BusInfo({ vector<string>{"s1", "s 2", "s3"}, false }));
	ASSERT_EQUAL(guider.CheckBuses().at("bus 1"), BusInfo({ vector<string>{"s1", "s 2", "s4", "s1"}, true }));
}

void TestProcessGetBusInfoQuery() {
	TransportGuider guider;

}

void MainTest() {
	stringstream out("");
	TransportGuider guider;

	vector<QueryPtr> queries;
	queries.push_back(make_unique<StopQuery>("Tolstopaltsevo", 55.611087, 37.20829));
	queries.push_back(make_unique<StopQuery>("Marushkino", 55.595884, 37.209755));
	queries.push_back(make_unique<BusStopsQuery>("256", vector<string>{"Biryulyovo Zapadnoye", "Biryusinka",
		"Universam", "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya", "Biryulyovo Zapadnoye"}, true));
	queries.push_back(make_unique<BusStopsQuery>("750", vector<string>{"Tolstopaltsevo", "Marushkino", "Rasskazovka"}, false));
	queries.push_back(make_unique<StopQuery>("Rasskazovka", 55.632761, 37.333324));
	queries.push_back(make_unique<StopQuery>("Biryulyovo Zapadnoye", 55.574371, 37.6517));
	queries.push_back(make_unique<StopQuery>("Biryusinka", 55.581065, 37.64839));
	queries.push_back(make_unique<StopQuery>("Universam", 55.587655, 37.645687));
	queries.push_back(make_unique<StopQuery>("Biryulyovo Tovarnaya", 55.592028, 37.653656));
	queries.push_back(make_unique<StopQuery>("Biryulyovo Passazhirskaya", 55.580999, 37.659164));
	queries.push_back(make_unique<GetBusInfoQuery>(string("256")));
	queries.push_back(make_unique<GetBusInfoQuery>(string("750")));
	queries.push_back(make_unique<GetBusInfoQuery>(string("751")));

	guider.ProcessQueries(move(queries), out);
	string expected = R"(Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length
Bus 750: 5 stops on route, 3 unique stops, 20939.5 route length
Bus 751: not found)";
	ASSERT_EQUAL(out.str(), expected);
}

void TestAll() {
	TestRunner tr;
	RUN_TEST(tr, TestCheckDelimiterType);
	RUN_TEST(tr, TestGetSeparatedToken);
	RUN_TEST(tr, TestConvertToDouble);
	RUN_TEST(tr, TestParsing);
	RUN_TEST(tr, TestReadQuerries);
	RUN_TEST(tr, TestProcessStopQuery);
	RUN_TEST(tr, TestProcessBusStopsQuery);
	RUN_TEST(tr, TestProcessGetBusInfoQuery);
	RUN_TEST(tr, MainTest);
}