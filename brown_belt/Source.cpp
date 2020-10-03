#include <map>
using namespace std;

class MyClass {
	void Foo() {
		// this ...
	}

	void Bar() const {
		// this ...
	}
};

int main() {
	map<int, int> cont;
	// ...
	map<int, int>::iterator it = cont.begin();
	auto& key = it->first;
	auto& value = it->second;
	return 0;
}