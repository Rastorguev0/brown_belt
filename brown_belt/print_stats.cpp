#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;
/*
enum class Gender {
	FEMALE,
	MALE
};

struct Person {
	int age;  // возраст
	Gender gender;  // пол
	bool is_employed;  // имеет ли работу
};

template <typename InputIt>
int ComputeMedianAge(InputIt range_begin, InputIt range_end) {
	if (range_begin == range_end) {
		return 0;
	}
	vector<typename InputIt::value_type> range_copy(range_begin, range_end);
	auto middle = begin(range_copy) + range_copy.size() / 2;
	nth_element(
		begin(range_copy), middle, end(range_copy),
		[](const Person& lhs, const Person& rhs) {
			return lhs.age < rhs.age;
		}
	);
	return middle->age;
}
*/
void PrintStat(int stat, string for_who) {
	cout << "Median age for " << for_who << " = " << stat << endl;
}

void PrintStats(vector<Person> persons) {
	cout << "Median age = " << ComputeMedianAge(persons.begin(), persons.end()) << endl;

	auto border = partition(persons.begin(), persons.end(), [](const Person& p) {
		return p.gender == Gender::FEMALE;
		});
	PrintStat(ComputeMedianAge(persons.begin(), border), "females");
	PrintStat(ComputeMedianAge(border, persons.end()), "males");

	auto border2 = partition(persons.begin(), border, [](const Person& p) {
		return p.is_employed;
		});
	PrintStat(ComputeMedianAge(persons.begin(), border2), "employed females");
	PrintStat(ComputeMedianAge(border2, border), "unemployed females");

	border2 = partition(border, persons.end(), [](const Person& p) {
		return p.is_employed;
		});
	PrintStat(ComputeMedianAge(border, border2), "employed males");
	PrintStat(ComputeMedianAge(border2, persons.end()), "unemployed males");
}
/*
int main() {
	vector<Person> persons = {
			{31, Gender::MALE, false},
			{40, Gender::FEMALE, true},
			{24, Gender::MALE, true},
			{20, Gender::FEMALE, true},
			{80, Gender::FEMALE, false},
			{78, Gender::MALE, false},
			{10, Gender::FEMALE, false},
			{55, Gender::MALE, true},
	};
	PrintStats(persons);
	return 0;
}
*/