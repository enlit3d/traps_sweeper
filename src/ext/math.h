#pragma once
#include "src/basic_types.h"

namespace engine::math
{
	constexpr number PI = 3.141592653589793238462643383279502884;

	template <typename T>
	T abs(T v) {
		return v < 0 ? -v : v;
	}

	f32 sin(f32 x);
	f32 cos(f32 x);
	f32 sqrt(f32 x);

	template <typename T>
	T min(T a, T b) {
		return a > b ? b : a;
	}

	template <typename T>
	T max(T a, T b) {
		return a > b ? a : b;
	}

	template <typename T>
	T clamp(T v, T min, T max) {
		return math::max(math::min(v, max), min);
	}

	f32 ceil(f32 x);
	f32 floor(f32 x);

	template <typename T>
	T round(T x) {
		return std::round(x);
	}

	template <typename Target_T, typename T>
	Target_T roundTo (T x) {
		return (Target_T)std::round(x);
	}

	template <typename T>
	T pow(T base, T exponent) {
		return (T)std::pow(base, exponent);
	}

	f32 lerp(f32 a, f32 b, f32 ratio);

	i32 toInt(f32 x);
	constexpr i32 toNextPowOf2(i32 x)
	{
		auto n = x;
		n--;
		n = n | (n >> 1);
		n = n | (n >> 2);
		n = n | (n >> 4);
		n = n | (n >> 8);
		n = n | (n >> 16);
		n++;
		return n;
	}

	u32 randBits();

	f32 randFloat(f32 min, f32 max);

	i32 randInt(i32 min, i32 maxInclusive);

	// fast math for pow of 2 types
	template <int N>
	constexpr bool isPowOf2() {
		return (N > 0 && ((N & (N - 1)) == 0));
	}
	template <int N>
	constexpr int highestBitPowOf2() {
		auto v = N;
		unsigned r = 0;

		while (v >>= 1) {
			r++;
		}

		return r;
	}
	template <int N>
	int fastDivPowOf2(int x) {
		static_assert(isPowOf2<N>(), "N is not pow of 2!");
		constexpr int shift = highestBitPowOf2<N>();
		return x >> shift;
	}
	template <int N>
	int fastModPowOf2(int x) {
		static_assert(isPowOf2<N>(), "N is not pow of 2!");
		return x & (N - 1);
	}

	template <int N>
	constexpr int toNextPowOf2() {
		auto n = N;
		n--;
		n = n | (n >> 1);
		n = n | (n >> 2);
		n = n | (n >> 4);
		n = n | (n >> 8);
		n = n | (n >> 16);
		n++;
		return n;
	}
}