#pragma once
#include "math.h"

namespace engine::math {
	
	template <typename Collection>
	Collection& shuffle(Collection& collection) {
		auto begin = collection.begin();
		auto end = collection.end();
		auto n = end - begin;
		for (auto i = 0; i < n-1; ++i) { // note the i < n-1
			auto j = math::randInt(i, n - 1); //j must satisfy i <= j < n
			std::swap(*(begin + i), *(begin + j));
		}
		return collection;
	}
}