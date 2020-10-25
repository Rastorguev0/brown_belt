#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <unordered_map>
#include <ctime>

using namespace std;

/*						DATE DECLARATION						*/
class Date {
public:
	Date(int year, int month, int day)
		: year_(year), month_(month), day_(day) {}

	time_t AsTimestamp() const;

	int GetDate() const;
private:
	int year_, month_, day_;
};

int ComputeDaysDiff(const Date& date_to, const Date& date_from) {
	const time_t timestamp_to = date_to.AsTimestamp();
	const time_t timestamp_from = date_from.AsTimestamp();
	static const int SECONDS_IN_DAY = 60 * 60 * 24;
	return static_cast<int>((timestamp_to - timestamp_from)) / SECONDS_IN_DAY;
}

/*						DATE DEFINITION						*/
time_t Date::AsTimestamp() const {
	std::tm t;
	t.tm_sec = 0;
	t.tm_min = 0;
	t.tm_hour = 0;
	t.tm_mday = day_;
	t.tm_mon = month_ - 1;
	t.tm_year = year_ - 1900;
	t.tm_isdst = 0;
	return mktime(&t);
}

int Date::GetDate() const {
	return ComputeDaysDiff(*this, { 2000, 1, 1 });
}

/*						BUDGET DEFINITION						*/
class Budget {
public:
	void ProcessQuery(string query) {
		stringstream q_stream(move(query));
		string cmd;
		if (q_stream >> cmd; cmd == "ComputeIncome") {
			double income = ComputeIncome(GetDate(q_stream), GetDate(q_stream));
			cout << setprecision(25) << income << endl;
		}
		else if (cmd == "Earn") {
			Date from = GetDate(q_stream);
			Date to = GetDate(q_stream);
			double value; q_stream >> value;
			Earn(from, to, value);
		}
		else if (cmd == "PayTax") {
			PayTax(GetDate(q_stream), GetDate(q_stream));
		} else throw logic_error("Unknown command!");
	}

	double ComputeIncome(const Date& from, const Date& to) {
		/*
		auto to_it = incomes.find(to.GetDate());
		double income = 0;
		for (auto from_it = incomes.find(from.GetDate()); from_it != to_it; from_it++) {
			income += from_it->second;
		}
		if (to_it != incomes.end()) income += to_it->second;
		return income;
		*/
		int int_to = to.GetDate();
		double income = 0;
		for (int int_from = from.GetDate(); int_from < int_to; int_from++) {
			income += incomes[int_from];
		}
		return income;
	}

	void Earn(const Date& from, const Date& to, double value) {
		int int_from = from.GetDate();
		int int_to = from.GetDate();
		if (int_from <= int_to) value /= (int_to - int_from + 1);
		while (int_from <= int_to) incomes[int_from++] += value;
	}

	void PayTax(const Date& from, const Date& to) {
		int int_to = to.GetDate();
		for (int int_from = from.GetDate(); int_from < int_to; int_from++) {
			incomes[int_from] *= 0.87;
		}
	}

private:
	unordered_map<int, double> incomes;

	static Date GetDate(stringstream& stream) {
		int y, m, d;
		stream >> y;
		stream.ignore(1) >> m;
		stream.ignore(1) >> d;
		return Date(y, m, d);
	}
};

int main() {
	stringstream ccin(R"(8
		Earn 2000 - 01 - 02 2000 - 01 - 06 20
		ComputeIncome 2000 - 01 - 01 2001 - 01 - 01
		PayTax 2000 - 01 - 02 2000 - 01 - 03
		ComputeIncome 2000 - 01 - 01 2001 - 01 - 01
		Earn 2000 - 01 - 03 2000 - 01 - 03 10
		ComputeIncome 2000 - 01 - 01 2001 - 01 - 01
		PayTax 2000 - 01 - 03 2000 - 01 - 03
		ComputeIncome 2000 - 01 - 01 2001 - 01 - 01)");

	int Q;
	ccin >> Q;
	string _; getline(ccin, _);
	Budget budget;

	for (int i = 0; i < Q; i++) {
		string query;
		getline(ccin, query);
		budget.ProcessQuery(move(query));
	}
	return 0;
}