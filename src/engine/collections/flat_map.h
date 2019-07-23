#pragma once

#include <vector>
namespace engine::collections
{
	template <typename Key, typename Value>
	class FlatMap {
	public:
		FlatMap() {};

		template <typename... Args>
		Value& emplace(Key key, Args&&... args) {
			this->_keys.push_back(key);
			return this->_values.emplace_back(args...);
		}

		bool has(Key key) const {
			for (auto k : this->_keys) {
				if (key == k) { return true; }
			}
			return false;
		}

		Value& get(Key key) {
			auto l = this->_keys.size();
			for (auto i = 0; i<l; ++i) {
				if (key == this->_keys[i]) {
					return this->_values[i];
				}
			}
			return this->_badValue;
		}

		std::vector<Value>& values() {
			return this->_values;
		}

		auto size() { return this->_keys.size(); }
	private:
		std::vector<Key> _keys = {};
		std::vector<Value> _values = {};
		Value _badValue = {};
	};

}