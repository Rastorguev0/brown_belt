#pragma once
#include <iostream>

namespace RAII {
	template<class Provider>
	class Booking {
	public:
		Booking(Provider* prov, int& cnt) : provider(prov), counter(cnt) {}

		Booking(const Booking&) = delete;
		Booking& operator = (const Booking&) = delete;

		Booking(Booking&& other) : provider(other.provider), counter(other.counter) {
			other.provider = nullptr;
		}
		Booking& operator = (Booking&& other) {
			delete provider;
			provider = other.provider;
			other.provider = nullptr;
			return *this;
		}

		~Booking() {
			if (provider) provider->CancelOrComplete(*this);
		}

	private:
		Provider* provider;
		int& counter;
	};
}