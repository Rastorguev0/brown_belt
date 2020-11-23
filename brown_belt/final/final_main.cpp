#include <iostream>
#include <vector>
#include "unit_tests.h"
#include "input_parsing.h"
#include "guider.h"

using namespace std;

int main() {
	//TestAll();
	vector<QueryPtr> queries = ReadQueries();
	TransportGuider guider;
	guider.ProcessQueries(move(queries));
	return 0;
}