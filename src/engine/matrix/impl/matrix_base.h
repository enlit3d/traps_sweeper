#pragma once

#include "src/basic_types.h"
#include <array>
namespace engine::matrix
{
	template <typename VecT, typename T, i32 N>
	class MatrixBase {
	public:
		template <typename... Args>
		MatrixBase(Args... args) 
			: _values{ (args)... } 
		{
			static_assert(sizeof...(Args) == N || sizeof...(Args) == 0);
		};
		template <i32 Size>
		MatrixBase& setTo(const T(&list)[Size]) {
			static_assert(Size == N);
			for (auto i = 0; i < N; ++i) {
				_values[i] = list[i];
			}
			return *this;
		}
		T& operator[](i32 index) { return _values[index]; }
		const T& operator[](i32 index) const { return _values[index]; };
		
		MatrixBase& identity() {
			for (auto i = 0; i < N; ++i) {
				_values[i] = 0.0f;
			}
			return *this;
		}

		T dot(const MatrixBase& other) const {
			T result = T{};
			for (auto i = 0; i < N; ++i) {
				result += _values[i] * other[i];
			}

			return result;
		}
		MatrixBase& operator+= (const MatrixBase& other) {
			for (auto i = 0; i < N; ++i) {
				_values[i] += other[i];
			}
			return *this;
		}
		MatrixBase& operator-= (const MatrixBase& other) {
			for (auto i = 0; i < N; ++i) {
				_values[i] -= other[i];
			}
			return *this;
		}
		MatrixBase& operator*= (const MatrixBase& other) {
			for (auto i = 0; i < N; ++i) {
				_values[i] *= other[i];
			}
			return *this;
		}
		MatrixBase& operator/= (const MatrixBase& other) {
			for (auto i = 0; i < N; ++i) {
				_values[i] /= other[i];
			}
			return *this;
		}
		MatrixBase& operator+= (T value) {
			for (auto& v : _values) {
				v += value;
			}
			return *this;
		}
		MatrixBase& operator-= (T value) {
			for (auto& v : _values) {
				v -= value;
			}
			return *this;
		}
		MatrixBase& operator*= (T value) {
			for (auto& v : _values) {
				v *= value;
			}
			return *this;
		}
		MatrixBase& operator/= (T value) {
			for (auto& v : _values) {
				v /= value;
			}
			return *this;
		}

		friend VecT operator+(VecT a, const VecT& b) { // is it possible to move these friend methods to .cpp file? I wasn't able to do it...
			a += b;
			return static_cast<VecT>(a);
		}

		friend VecT operator-(VecT a, const VecT& b) {
			a -= b;
			return static_cast<VecT>(a);
		}

		friend VecT operator*(VecT a, const VecT& b) {
			a *= b;
			return static_cast<VecT>(a);
		}

		friend VecT operator/(VecT a, const VecT& b) {
			a /= b;
			return static_cast<VecT>(a);
		}

		friend VecT operator+(VecT a, T b) {
			a += b;
			return static_cast<VecT>(a);
		}

		friend VecT operator-(VecT a, T b) {
			a -= b;
			return static_cast<VecT>(a);
		}

		friend VecT operator*(VecT a, T b) {
			a *= b;
			return static_cast<VecT>(a);
		}

		friend VecT operator/(VecT a, T b) {
			a /= b;
			return static_cast<VecT>(a);
		}

		auto begin() { return _values.begin(); }
		auto end() { return _values.end(); }
		const auto begin() const { return _values.begin(); }
		const auto end() const { return _values.end(); }
		T* data() { return _values.data(); }
		const T* data() const { return _values.data(); }
	private:
		std::array<T, N> _values = {};
	};
}