#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "test_runner.h"

using namespace std;

bool IsSubdomain(string_view subdomain, string_view domain) {
  auto i = subdomain.size() - 1;
  auto j = domain.size() - 1;
  while (i >= 0 && j >= 0) {
    if (subdomain[i--] != domain[j--]) {
      return false;
    }
  }
  return (i < 0 && domain[j] == '.')
      || (j < 0 && subdomain[i] == '.');
}


vector<string> ReadDomains(istream& input) {
  size_t count;
  input >> count;

  vector<string> domains(count);
  for (size_t i = 0; i < count; ++i) {
    string domain;
    getline(input, domain);
    domains.push_back(move(domain));
  }
  return domains;
}


int main() {
  const vector<string> banned_domains = ReadDomains();
  const vector<string> domains_to_check = ReadDomains();

  for (string_view domain : banned_domains) {
    reverse(begin(domain), end(domain));
  }
  sort(begin(banned_domains), end(banned_domains));

  size_t insert_pos = 0;
  for (string& domain : banned_domains) {
    if (insert_pos == 0 || !IsSubdomain(domain, banned_domains[insert_pos - 1])) {
      swap(banned_domains[insert_pos++], domain);
    }
  }
  banned_domains.resize(insert_pos);

  for (const string_view domain : domains_to_check) {
    if (const auto it = upper_bound(begin(banned_domains), end(banned_domains), domain);
        it != begin(banned_domains) && IsSubdomain(domain, *prev(it))) {
      cout << "Good" << endl;
    } else {
      cout << "Bad" << endl;
    }
  }
  return 0;
}
