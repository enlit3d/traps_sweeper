#pragma once

#include <type_traits>
#include <new>

namespace engine::collections {

	/*
		A "vector" with fixed capacity. All elements are embedded within this data structure without any dynamic allocation.
		Essentially a buffer + size
	*/
	template <typename T, i32 N>
	class FixedVector {
	public:
		using value_type        = T;
		using const_value_type  = const T;
		using pointer           = T*;
		using const_pointer     = const T*;
		using reference         = T&;
		using const_reference   = const T&;
		using iterator          = T*;
		using const_iterator    = const T*;
	public:
		FixedVector() {}
		~FixedVector() {
			for (auto& elem : *this) {
				elem.~T();
			}
		}

		FixedVector(FixedVector&& other) noexcept {
			for (auto i = 0; i < other._size; ++i) {
				new(&_data[i])T(std::move(other[i]));
			}
			_size = other._size;
		}
		FixedVector& operator=(FixedVector&& other) noexcept {
			if (this == &other) { return *this; }
			this->~FixedVector();
			for (auto i = 0; i < other._size; ++i) {
				new(&_data[i])T(std::move(other[i]));
			}
			_size = other._size;
			
			return *this;
		}

		FixedVector(const FixedVector& other) {
			for (auto i = 0; i < other._size; ++i) {
				new(&_data[i])T(other[i]);
			}
			_size = other._size;
		};
		FixedVector& operator=(const FixedVector&) = delete;

		template <typename... Args>
		void emplace_back(Args&& ... args) {
			if (_size >= N) { throw; }
			new(&_data[_size]) T(std::forward<Args>(args)...);
			++_size;
		}

		T& operator[](i32 index) {
			if (index < 0 || index >= _size) { throw; }
			return *(data() + index);
		}

		const T& operator[](i32 index) const {
			if (index < 0 || index >= _size) { throw; }
			return *(data() + index);
		}

		iterator begin()                  noexcept { return data(); }
		const_iterator begin()      const noexcept { return data(); }
		const_iterator cbegin()     const noexcept { return data(); }
		iterator end()                    noexcept { return data() + _size; }
		const_iterator end()        const noexcept { return data() + _size; }
		const_iterator cend()       const noexcept { return data() + _size; }

		T* data() {
			return reinterpret_cast<T*>(&_data[0]);
			//return std::launder(reinterpret_cast<T*>(&_data[0]));
		}

		const T* data() const {
			return reinterpret_cast<const T*>(&_data[0]);
			//return std::launder(reinterpret_cast<const T*>(&_data[0]));
		}	

		friend bool operator==(const FixedVector& a, const FixedVector& b) {
			if (a._size != b._size) { return false; }
			for (auto i = 0; i < a._size; ++i) {
				if (!(a[i] == b[i])) { return false; }
			}
			return true;
		}

		i32 size() const { return _size; }

	private:
		typename std::aligned_storage<sizeof(T), alignof(T)>::type _data[N] = {};
		i32 _size = 0;
	};
}