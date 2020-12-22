#pragma once
#include "test_runner.h"
#include "input_parsing.h"
#include "guider.h"
#include "graph.h"
#include "router.h"
#include "profile.h"
#include <fstream>
using namespace std;

void Test1() {
	ifstream input("final\\input4.json");
	ofstream out("final\\log.txt");
	LOG_DURATION("Final test");
	if (input.is_open()) {
		vector<QueryPtr> queries;
		{
			LOG_DURATION("READING");
			queries = ReadQueries(input);
		}
		TransportGuider guider;
		guider.ProcessQueries(move(queries), out);
	}
}

void TestAll() {
	TestRunner tr;
	RUN_TEST(tr, Test1);
}