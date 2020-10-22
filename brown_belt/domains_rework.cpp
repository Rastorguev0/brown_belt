#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <string_view>
#include <set>
#include <iterator>

#include "test_runner.h"

using namespace std;

class DomainChecker {
public:
	explicit DomainChecker(vector<string> domains) {
		for (auto& domain : domains) {
			reverse(domain.begin(), domain.end());
			auto it = banned_domains.upper_bound(domain);
			if (it == banned_domains.begin()) {
				if (it == banned_domains.end()) {
					banned_domains.insert(domain);
					continue;
				}
				else it = next(it);
			}
			if (IsSubdomain(domain, *prev(it))) {
				if (domain.size() < prev(it)->size()) {
					banned_domains.erase(prev(it));
				}
				else continue;
			}
			banned_domains.insert(move(domain));
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

	vector<bool> CheckDomains(const vector<string>& domains) {
		vector<bool> result(domains.size());
		for (size_t i = 0; i < domains.size(); i++) {
			result[i] = BadDomain(domains[i]);
		}
		return result;
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

void ResultOutput(ostream& out, const vector<bool>& bools) {
	for (const auto& result : bools) {
		out << (result ? "Bad\n" : "Good\n");
	}
}

vector<bool> Test(istringstream& input) {
	DomainChecker domain_checker(ReadDomains(input));
	const vector<string> domains_to_check = ReadDomains(input);
	auto result = domain_checker.CheckDomains(domains_to_check);
	return result;
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
	ASSERT_EQUAL(Test(input), vector<bool>({ 1, 1, 1, 1, 1, 0, 0 }));
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
	ASSERT_EQUAL(Test(input), vector<bool>({0, 0, 1}));
}

void Test7() {
	istringstream input(R"(
2
b.c
a.b.c
1
d.b.c
)");
	ASSERT_EQUAL(Test(input), vector<bool>({ 1 }));
}

void Test8() {
	istringstream input(R"(
3
dot.com
ozon.ru
com
1
aorus.com
)");
	ASSERT_EQUAL(Test(input), vector<bool>({ 1 }));
}

void TestSmth() {
	{
		istringstream input(R"(
4
earth
fire
water
air
8
yandex.ru
test
earthshaker
watermelon
google.air
googleair
fireonme
.fire
)");
		ASSERT_EQUAL(Test(input), vector<bool>({ 0, 0, 0, 0, 1, 0, 0, 1 }));
	}

	{
		istringstream input(R"(
1
com
9
com
dotcom.com
dotcom
com.dotcom
com.com
.com
com.ru
moc
comcom.moc
)");
		ASSERT_EQUAL(Test(input), vector<bool>({1, 1, 0, 0, 1, 1, 0, 0, 0}));
	}

	{
		istringstream input(R"(
1
m.vk.com
1
vk.com
)");
		ASSERT_EQUAL(Test(input), vector<bool>({0}));
	}

	{
		istringstream input(R"(
5
earth
fire
water
air
*
8
yandex.ru
test
earthshaker
watermelon
google.air
googleair
fireonme
.fire
)");
		ASSERT_EQUAL(Test(input), vector<bool>({ 0, 0, 0, 0, 1, 0, 0, 1 }));
	}

	{
		istringstream input(R"(
2
b.c
a.b.c
1
d.b.c
)");
		ASSERT_EQUAL(Test(input), vector<bool>({ 1 }));
	}

	{
		istringstream input(R"(
1
com
3
com.ru
ru.com.en
ru.com
)");
		ASSERT_EQUAL(Test(input), vector<bool>({ 0, 0, 1 }));
	}
	
	{
		istringstream input(R"(
9
yandex.ru
xxx.ru
ru.x
ry.yandex
xr.u
x.ru
ru.com
x.ru.ru
x.ru.x
9
yap.com
ao.ru.x
xcru
xru
xrux.ru
uandexxx.rx
ru.x.ru
ru.x
x.ru
)");
		//x.ru --- ru.x --- ry.yandex --- xr.u --- ru.com --- x.ru.ru
		ASSERT_EQUAL(Test(input), vector<bool>({ 0, 1, 0, 0, 0, 0, 1, 1, 1 }));
	}
	{
		istringstream input(R"(
12
a
c.a
c.a
a.c
a
a
a.a
a.b.c
a.a
c.a
a
c.b.a
1
ya.ru
)");
		ASSERT_EQUAL(Test(input), vector<bool>({ 0 }));
	}
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, Test1);
	RUN_TEST(tr, Test5);
	RUN_TEST(tr, Test7);
	RUN_TEST(tr, Test8);
	RUN_TEST(tr, TestSmth);
	/*
	DomainChecker domain_checker(ReadDomains(cin));
	const vector<string> domains_to_check = ReadDomains(cin);
	auto result = domain_checker.CheckDomains(domains_to_check);
	ResultOutput(cout, result);
	*/
	return 0;
}