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
	return (timestamp_to - timestamp_from) / SECONDS_IN_DAY;
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
		q_stream >> cmd;
		Date from = GetDate(q_stream);
		Date to = GetDate(q_stream);

		if (cmd == "ComputeIncome") {
			cout << setprecision(25) << ComputeIncome(from, to) << endl;
		}
		else if (cmd == "Earn") {
			double value; q_stream >> value;
			Earn(from, to, value);
		}
		else if (cmd == "PayTax") {
			double percent; q_stream >> percent;
			PayTax(from, to, percent);
		}
		else if (cmd == "Spend") {
			double value; q_stream >> value;
			Spend(from, to, value);
		}
		else throw logic_error("Unknown command!");
	}

	double ComputeIncome(const Date& from, const Date& to) {
		auto [int_from, int_to] = GetDateRange(from, to);
		double income = 0;
		while (int_from <= int_to) {
			income += incomes[int_from++].Total();
		}
		return income;
	}

	void Spend(const Date& from, const Date& to, double value) {
		auto [int_from, int_to] = GetDateRange(from, to);
		value /= (abs(int_to - int_from) + 1);
		while (int_from <= int_to) incomes[int_from++].spent += value;
	}

	void Earn(const Date& from, const Date& to, double value) {
		auto [int_from, int_to] = GetDateRange(from, to);
		value /= (abs(int_to - int_from) + 1);
		while (int_from <= int_to) incomes[int_from++].earned += value;
	}

	void PayTax(const Date& from, const Date& to, double percent) {
		auto [int_from, int_to] = GetDateRange(from, to);
		while (int_from <= int_to) {
			incomes[int_from++].earned *= (1 - percent / 100);
		}
	}

public:
	struct Money {
		double Total() const {
			return earned - spent;
		}
		double spent = 0;
		double earned = 0;
	};

private:
	unordered_map<int, Money> incomes;

	static Date GetDate(stringstream& stream) {
		int y, m, d;
		stream >> y;
		stream.ignore(1) >> m;
		stream.ignore(1) >> d;
		return Date(y, m, d);
	}

	pair<int, int> GetDateRange(const Date& from, const Date& to) {
		return { from.GetDate(), to.GetDate() };
	}
};

int main() {
	stringstream ccin(R"(8
Earn 2000-01-02 2000-01-06 20
ComputeIncome 2000-01-01 2001-01-01
PayTax 2000-01-02 2000-01-03 13
ComputeIncome 2000-01-01 2001-01-01
Spend 2000-12-30 2001-01-02 14
ComputeIncome 2000-01-01 2001-01-01
PayTax 2000-12-30 2000-12-30 13
ComputeIncome 2000-01-01 2001-01-01)");

	int Q;
	cin >> Q;
	string _; getline(cin, _);
	Budget budget;

	for (int i = 0; i < Q; i++) {
		string query;
		getline(cin, query);
		budget.ProcessQuery(move(query));
	}
	return 0;
}