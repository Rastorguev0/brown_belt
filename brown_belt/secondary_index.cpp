#include "test_runner.h"

#include <iostream>
#include <tuple>
#include <cmath>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace std;

struct Record {
	string id;
	string title;
	string user;
	int timestamp;
	int karma;
};

bool operator == (const Record& lhs, const Record& rhs) {
	return tie(lhs.id, lhs.title, lhs.user, lhs.timestamp, lhs.karma)
		== tie(rhs.id, rhs.title, rhs.user, rhs.timestamp, rhs.karma);
}

struct RecordHasher {
	size_t operator() (const Record& r) const {
		hash<string> str_hasher;
		hash<int> int_hasher;
		size_t A = str_hasher(r.id);
		size_t B = str_hasher(r.title);
		size_t C = str_hasher(r.user);
		size_t D = int_hasher(r.timestamp);
		size_t E = int_hasher(r.karma);

		size_t x = 997;
		return static_cast<size_t>((A * pow(x, 5) + B * pow(x, 4) + C * pow(x, 3) + D * pow(x, 2) + E));
	}
};

// Реализуйте этот класс
class Database {
public:
	bool Put(const Record& record) {
		if (storage.count(record.id)) {
			return false;
		}
		else {
			storage[record.id] = Data(record);
			Data& data = storage[record.id];
			data.time = time_map.insert({ record.timestamp, &data.record });
			data.karma = karma_map.insert({ record.karma, &data.record });
			data.user = user_map.insert({ record.user, &data.record });
			return true;
		}
	}

	const Record* GetById(const string& id) const {
		return storage.count(id) ? &storage.at(id).record : nullptr;
	}

	bool Erase(const string& id) {
		if (!storage.count(id)) {
			return false;
		}
		else {
			Data& data = storage[id];
			time_map.erase(data.time);
			karma_map.erase(data.karma);
			user_map.erase(data.user);
			storage.erase(id);
			return true;
		}
	}

	template <typename Callback>
	void RangeByTimestamp(int low, int high, Callback callback) const {
		auto begin = time_map.lower_bound(low);
		auto end = time_map.upper_bound(high);
		while (begin != end) {
			if (!callback(*(begin->second))) return;
			begin++;
		}
	}

	template <typename Callback>
	void RangeByKarma(int low, int high, Callback callback) const {
		auto begin = karma_map.lower_bound(low);
		auto end = karma_map.upper_bound(high);
		while (begin != end) {
			if (!callback(*(begin->second))) return;
			begin++;
		}
	}

	template <typename Callback>
	void AllByUser(const string& user, Callback callback) const {
		auto begin = user_map.lower_bound(user);
		auto end = user_map.upper_bound(user);
		while (begin != end) {
			if (!callback(*(begin->second))) return;
			begin++;
		}
	}
private:
	struct Data {
		Data() {}
		Data(const Record& r) {
			record = r;
			time = karma = {};
			user = {};
		}
		Record record;
		multimap<int, const Record*>::iterator time;
		multimap<int, const Record*>::iterator karma;
		multimap<string, const Record*>::iterator user;
	};

	unordered_map<string, Data> storage;
	multimap<int, const Record*> time_map;
	multimap<int, const Record*> karma_map;
	multimap<string, const Record*> user_map;
};

void TestRangeBoundaries() {
	const int good_karma = 1000;
	const int bad_karma = -10;

	Database db;
	db.Put({ "id1", "Hello there", "master", 1536107260, good_karma });
	db.Put({ "id2", "O>>-<", "general2", 1536107260, bad_karma });

	int count = 0;
	db.RangeByKarma(bad_karma, good_karma, [&count](const Record&) {
		++count;
		return true;
		});

	ASSERT_EQUAL(2, count);
}

void TestSameUser() {
	Database db;
	db.Put({ "id1", "Don't sell", "master", 1536107260, 1000 });
	db.Put({ "id2", "Rethink life", "master", 1536107260, 2000 });

	int count = 0;
	db.AllByUser("master", [&count](const Record&) {
		++count;
		return true;
		});

	ASSERT_EQUAL(2, count);
}

void TestReplacement() {
	const string final_body = "Feeling sad";

	Database db;
	db.Put({ "id", "Have a hand", "not-master", 1536107260, 10 });
	db.Erase("id");
	db.Put({ "id", final_body, "not-master", 1536107260, -10 });

	auto record = db.GetById("id");
	ASSERT(record != nullptr);
	ASSERT_EQUAL(final_body, record->title);
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestRangeBoundaries);
	RUN_TEST(tr, TestSameUser);
	RUN_TEST(tr, TestReplacement);
	return 0;
}