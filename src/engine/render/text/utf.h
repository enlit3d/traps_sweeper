#pragma once
#include "src/basic_types.h"
#include <cassert>

namespace engine::ext::utf {
	using UTF8Raw = const char*;

	constexpr i32 numOctets(u8 c) {
		if ((c & 0x80) == 0)          // lead bit is zero, must be a single ascii
			return 1;
		else if ((c & 0xE0) == 0xC0)  // 110x xxxx
			return 2;
		else if ((c & 0xF0) == 0xE0) // 1110 xxxx
			return 3;
		else if ((c & 0xF8) == 0xF0) // 1111 0xxx
			return 4;
		else {
			//std::cout << "Unrecognized lead byte " << c << "\n";
			throw;
		}
	}

	constexpr u32 utf8ToUtf32(UTF8Raw c) {
		auto octets = numOctets(c[0]);
		auto text = c;

		//constexpr u32 UTF8_THREE_BYTES_MASK = 0xF0;
		//constexpr u32 UTF8_CONTINUATION_BITS = 0xC0;
		//constexpr u32 UTF8_FOUR_BYTES_MASK = 0xF8;

		switch (octets) {
		case 1:
			return c[0] & 0x7F;
		case 2:
			return (c[0] & 0x1F) << 6 | (c[1] & 0x3F);
		case 3:
			return (c[0] & 0x0F) << 12 | (c[1] & 0x3F) << 6 | (c[2] & 0x3F);
		case 4:
			return (c[0] & 0x07) << 18 | (c[1] & 0x3F) << 12 | (c[2] & 0x3F) << 6 | (c[3] & 0x3F); 
		default:
			throw;
		}
	}

	class UTF8Iter {
	public:
		UTF8Iter(UTF8Raw ptr)
			: _ptr(ptr)
		{}

		UTF8Iter& operator++() {
			_ptr += _buffer[NUM_OCTETS_LOC];
			return *this;
		}
		bool operator!=(const UTF8Iter& other) const {
			return _ptr != other._ptr;
		}
		UTF8Raw operator*() {
			auto octets = numOctets(*_ptr);
			for (auto i = 0; i < octets; ++i) {
				_buffer[i] = *(_ptr + i);
			}
			_buffer[octets] = '\0';
			_buffer[NUM_OCTETS_LOC] = octets;
			return _buffer.data();
		}
	private:
		UTF8Raw _ptr;
		std::array<char, 6> _buffer = {}; // utf8 have at most 4 bytes for each character, then need 1 more for '\0', 6th byte is used to store the number of octets for the current character
		constexpr static auto NUM_OCTETS_LOC = 5;
	};

	class UTF8Stream {
	public:
		UTF8Stream(UTF8Raw src)
		{
			auto cp = 0;
			auto i = 0;
			while (true) {
				if (src[i] == '\0' || i > 65000) { break; }
				auto c = numOctets(src[i]);
				i += c;
				++cp;
			}

			_begin = src;
			_end = src + i;
			_count = cp;
		}
		UTF8Stream(const UTF8Stream& stream, i32 startIndex, i32 endIndex) {
			assert(startIndex >= 0 && startIndex < stream.count() && (endIndex >= startIndex) && endIndex <= stream.count());
			auto i = 0;
			for (auto a = 0; a < startIndex;++a) {
				i += numOctets(stream._begin[i]);
			}
			_begin = stream._begin + i;

			auto c = 0;
			auto diff = endIndex - startIndex;
			while (diff-- > 0) {
				i += numOctets(stream._begin[i]);
				c++;
			}
			_end = stream._begin + i;
			_count = c;
		}
		i32 count() const { // returns # of codepoints
			return _count;
		}
		UTF8Iter begin() {
			return UTF8Iter{ _begin };
		}
		UTF8Iter end() {
			return UTF8Iter{ _end };
		}
	private:
		UTF8Raw _begin;
		UTF8Raw _end;
		i32 _count;
	};
}