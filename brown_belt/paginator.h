#pragma once
#include <numeric>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

template<class Iterator>
class IteratorRange {
private:
	Iterator begin_it, end_it;
public:
	IteratorRange(Iterator b, Iterator e) {
		begin_it = b;
		end_it = e;
	}
	Iterator begin() const {
		return begin_it;
	}
	Iterator end() const {
		return end_it;
	}
	size_t size() const {
		return end_it - begin_it;
	}
};

template<class It>
auto MakeRange(It begin, It end) {
	return IteratorRange(begin, end);
}

template <typename Iterator>
class Paginator
{
private:
	vector<IteratorRange<Iterator>> pages;

public:
	Paginator(Iterator begin, Iterator end, size_t page_size) {
		if (begin != end) {
			size_t pos = 1;

			Iterator first = begin;
			Iterator it = next(begin);
			while (it != end) {
				if (pos % page_size == 0) {
					pages.push_back({ first, it });
					first = it;
				}
				++pos;
				++it;
			}
			if (first != end) pages.push_back({ first, end });
		}
	}

	auto begin() const {
		return pages.begin();
	}

	auto end() const {
		return pages.end();
	}

	size_t size() const {
		return pages.size();
	}
};

template <typename C>
auto Paginate(C& c, size_t page_size) {
	return Paginator(c.begin(), c.end(), page_size);
}