#pragma once
#include "input_parsing.h"
#include <unordered_map>
#include <string>
using namespace std;

struct StopInfo {
	Coordinates coords;
};

class TransportGuider {
public:
	TransportGuider() = default;
	void ProcessQueries(vector<QueryPtr> queries);
protected:
	unordered_map<string, StopInfo> stops_info;
};