#pragma once
#include <array>

namespace engine::collections
{
	/*
	* A constexpr bitset that only implements a few methods. The std::Bitset is not constexpr...
	*/
	template <int N>
	class BitSet {
	private:
		using elem_t = u32;
		constexpr static auto BITS_PER_ELEM = sizeof(elem_t) * 8;
		constexpr static auto ARR_SIZE = (N + (BITS_PER_ELEM - 1)) / BITS_PER_ELEM;
		using array_t = std::array<elem_t, ARR_SIZE>;
	public:
		constexpr BitSet() {};
		constexpr BitSet& set(int index, bool value) noexcept {
			auto elemIndex = index / BITS_PER_ELEM;
			auto bitIndex = index - elemIndex * BITS_PER_ELEM;
			if (value) {
				data[elemIndex] |= (static_cast<elem_t>(1) << bitIndex);
			}
			else {
				data[elemIndex] &= ~(static_cast<elem_t>(1) << bitIndex);
			}
			return *this;
		}
		constexpr bool operator[] (int index) const {
			auto elemIndex = index / BITS_PER_ELEM;
			auto bitIndex = index - elemIndex * BITS_PER_ELEM;
			return data[elemIndex] &= (static_cast<elem_t>(1) << bitIndex);
		}
		constexpr bool operator== (const BitSet& other) const {
			return this->data == other.data;
		}
		constexpr bool operator!= (const BitSet& other) const {
			return !(*this == other);
		}
		constexpr BitSet operator& (const BitSet& other) const {
			BitSet out{ *this };
			for (auto i = 0; i < ARR_SIZE; ++i) {
				out.data[i] &= other.data[i];
			}
			return out;
		}

	private:
		array_t data = {};
	};
}