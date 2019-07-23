#pragma once

#include <vector>
#include <algorithm>
namespace engine::collections
{
	template <typename T, typename Compare = std::less<T>>
	class SortedVector {
	public:
		SortedVector() {};
		void insert(T value) {
			auto p = std::lower_bound(begin(), end(), value, Compare());
			_values.insert(p, value);
		}
		void erase(typename std::vector<T>::const_iterator it) {
			return _values.erase(it);
		}
		auto find(const T& value) {
			auto p = std::lower_bound(begin(), end(), value, Compare());
			if (p!= end() && *p != value) { return end(); }
			else { return p; }
		}
		auto begin() { return _values.begin(); }
		auto end() { return _values.end(); }
		const auto begin() const { return _values.begin(); }
		const auto end() const { return _values.end(); }
	private:
		std::vector<T> _values = {};
	};

}