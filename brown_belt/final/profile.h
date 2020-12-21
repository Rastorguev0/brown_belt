#pragma once

#include <chrono>
#include <iostream>
#include <string>

using namespace std;
using namespace std::chrono;

struct Timer {
    Timer(const string& m) : msg(m) {}
    Timer() {}
    long long time = 0;
    string msg = "";
};

class LogDuration {
public:
    explicit LogDuration(const string& msg = "")
        : message(msg + ": ")
        , start(steady_clock::now())
    {
    }

    ~LogDuration() {
        auto finish = steady_clock::now();
        auto dur = finish - start;
        cerr << message
            << duration_cast<milliseconds>(dur).count()
            << " ms" << endl;
    }
private:
    string message;
    steady_clock::time_point start;
};

class GetDuration {
public:
    explicit GetDuration(Timer& t)
        : start(steady_clock::now()),
        timer(t)
    {
    }

    ~GetDuration() {
        auto finish = steady_clock::now();
        auto dur = finish - start;
        timer.time += duration_cast<milliseconds>(dur).count();
    }
private:
    steady_clock::time_point start;
    Timer& timer;
};

#define UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
#define UNIQ_ID(lineno) UNIQ_ID_IMPL(lineno)

#define GET_DURATION(timer) \
  GetDuration UNIQ_ID(__LINE__){timer};

#define LOG_DURATION(message) \
  LogDuration UNIQ_ID(__LINE__){message};
