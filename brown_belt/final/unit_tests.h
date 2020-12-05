#pragma once
#include "test_runner.h"
#include "input_parsing.h"
#include "guider.h"
#include "graph.h"
#include "router.h"
#include <fstream>
using namespace std;

void Test1() {
	ifstream input("final\\input1.json");
	if (input.is_open()) {
		vector<QueryPtr> queries = ReadQueries(input);
		TransportGuider guider;
		guider.ProcessQueries(move(queries));
	}
}

void TestAll() {
	TestRunner tr;
	RUN_TEST(tr, Test1);
}