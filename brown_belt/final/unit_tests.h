#pragma once
#include "test_runner.h"

using namespace std;

void Test1() {

}

void Test2() {

}

void Test3() {

}

void TestAll() {
	TestRunner tr;
	RUN_TEST(tr, Test1);
	RUN_TEST(tr, Test3);
	RUN_TEST(tr, Test2);
}