#include <iostream>
#include <vector>
#include "unit_tests.h"
#include "input_parsing.h"

using namespace std;

int main() {
	TestAll();
	vector<Query> queries = ReadQueries();
	return 0;
}