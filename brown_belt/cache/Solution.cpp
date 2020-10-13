#include "Common.h"
#include <unordered_map>
#include <algorithm>
#include <mutex>
#include <atomic>

using namespace std;

class LruCache : public ICache {
public:
	LruCache(
		shared_ptr<IBooksUnpacker> books_unpacker,
		const Settings& settings
	) : books_unpacker_(books_unpacker), settings_(settings) {}

	BookPtr GetBook(const string& book_name) override {
		lock_guard lock(mutex_);
		if (cached_books_.count(book_name)) return cached_books_[book_name].ptr;
		auto u_book_ptr = books_unpacker_->UnpackBook(book_name);
		bool can_cache_book = DumpCacheForBook(u_book_ptr->GetContent().size());
		MaxRatingUpdate();

		BookPtr book = move(u_book_ptr);
		if (can_cache_book) {
			cached_books_[book_name] = { book, ++max_rating_ };
			cache_memory_ += book->GetContent().size();
		}
		return move(book);
	}
private:
	void MaxRatingUpdate() {
		auto it = max_element(cached_books_.begin(), cached_books_.end(), Comparator());
		if (it != cached_books_.end()) max_rating_ = it->second.rating;
		else max_rating_ = 0;
	}

	void DumpRareBook() {
		if (!cached_books_.empty()) {
			auto it = min_element(cached_books_.begin(), cached_books_.end(), Comparator());
			cache_memory_ -= it->second.ptr->GetContent().size();
			cached_books_.erase(it);
		}
	}

	bool DumpCacheForBook(size_t book_size) {
		if (book_size > settings_.max_memory) return false;
		else {
			while (cache_memory_ + book_size > settings_.max_memory) {
				DumpRareBook();
			}
			return true;
		}
	}

private:
	struct BookInfo {
		BookPtr ptr;
		int rating = 0;
	};
	struct Comparator {
		bool operator()(const pair<string, BookInfo>& lhs,
			const pair<string, BookInfo>& rhs) const {
			return lhs.second.rating < rhs.second.rating;
		}
	};

	atomic<int>max_rating_ = 0;
	atomic<int> cache_memory_ = 0;

private:
	shared_ptr<IBooksUnpacker> books_unpacker_;
	const Settings settings_;
	unordered_map<string, BookInfo> cached_books_;
	mutable mutex mutex_;
};


unique_ptr<ICache> MakeCache(
	shared_ptr<IBooksUnpacker> books_unpacker,
	const ICache::Settings& settings
) {
	return make_unique<LruCache>(books_unpacker, settings);
}
