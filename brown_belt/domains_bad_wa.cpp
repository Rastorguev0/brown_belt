#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <set>
#include <iterator>

#include "test_runner.h"

using namespace std;

class DomainChecker {
public:
	explicit DomainChecker(vector<string> domains) {
		for (auto& domain : domains) {
			if (!BadDomain(domain)) {
				reverse(domain.begin(), domain.end());
				banned_domains.insert(move(domain));
			}
		}
	}

	bool IsSubdomain(string_view domain, string_view banned_domain) const {
		size_t min_size = min(domain.size(), banned_domain.size());
		bool is_subdomain = true;
		size_t i = 0;
		for (; i < min_size && is_subdomain; i++) {
			is_subdomain = domain[i] == banned_domain[i];
		}
		bool dot = (i < domain.size() ? domain[i] == '.' : true);
		return is_subdomain && dot;
	}

	bool BadDomain(const string& domain) const {
		string dom(rbegin(domain), rend(domain));
		const auto it = banned_domains.upper_bound(dom);
		return it != banned_domains.begin() ? IsSubdomain(dom, *prev(it)) : false;
	}

private:
	set<string> banned_domains;
};

vector<string> ReadDomains(istream& input) {
	size_t count;
	input >> count;
	string empty_string;
	getline(input, empty_string);

	vector<string> domains(count);
	for (size_t i = 0; i < count; ++i) {
		string domain;
		getline(input, domain);
		domains[i] = move(domain);
	}
	return move(domains);
}

vector<bool> CheckDomains(const DomainChecker& dc, const vector<string>& domains) {
	vector<bool> result(domains.size());
	for (size_t i = 0; i < domains.size(); i++) {
		result[i] = dc.BadDomain(domains[i]);
	}
	return result;
}

void ResultOutput(ostream& out, const vector<bool>& bools) {
	for (const auto& result : bools) {
		out << (result ? "Bad\n" : "Good\n");
	}
}

string Test(istringstream& input) {
	DomainChecker domain_checker(ReadDomains(input));
	const vector<string> domains_to_check = ReadDomains(input);
	auto result = CheckDomains(domain_checker, domains_to_check);
	stringstream out;
	ResultOutput(out, result);
	return out.str();
}

void Test1() {
	istringstream input(R"(
4
ya.ru
maps.me
m.ya.ru
com
7
ya.ru
ya.com
m.maps.me
moscow.m.ya.ru
maps.com
maps.ru
ya.ya
)");
	ASSERT_EQUAL(Test(input), "Bad\nBad\nBad\nBad\nBad\nGood\nGood\n");
}

void Test5() {
	istringstream input(R"(
1
com
3
com.ru
ru.com.en
ru.com
)");
	ASSERT_EQUAL(Test(input), "Good\nGood\nBad\n");
}

void Test7() {
	istringstream input(R"(
2
b.c
a.b.c
1
d.b.c
)");
	ASSERT_EQUAL(Test(input), "Bad\n");
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, Test1);
	RUN_TEST(tr, Test5);
	RUN_TEST(tr, Test7);
	DomainChecker domain_checker(ReadDomains(cin));
	const vector<string> domains_to_check = ReadDomains(cin);
	/*
	size_t insert_pos = 0;
	for (string& domain : banned_domains) {
		if (insert_pos == 0 || !IsSubdomain(domain, banned_domains[insert_pos - 1])) {
			swap(banned_domains[insert_pos++], domain);
		}
	}
	banned_domains.resize(insert_pos);
	*/
	auto result = CheckDomains(domain_checker, domains_to_check);
	ResultOutput(cout, result);
	return 0;
}