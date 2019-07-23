#pragma once
#include <cstdint>
#include <cstddef>

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using f64 = double;
using f32 = float;
using number = f64;

using index_t = i64;
using byte = unsigned char;

class NotCopyable {
public:
	NotCopyable() = default;
	// can be moved
	NotCopyable& operator=(NotCopyable&&) = default;
	NotCopyable(NotCopyable&&) = default;
protected:
	// cant be copied
	NotCopyable(const NotCopyable& that) = delete;
	NotCopyable& operator=(NotCopyable const&) = delete;
};

class NotCopyOrMoveable {
public:
	NotCopyOrMoveable() = default;
protected:
	// cant be copied
	NotCopyOrMoveable(const NotCopyOrMoveable& that) = delete;
	NotCopyOrMoveable& operator=(NotCopyOrMoveable const&) = delete;

	// cant be moved
	NotCopyOrMoveable(NotCopyOrMoveable&& that) = delete;
	NotCopyOrMoveable& operator=(NotCopyOrMoveable&& that) = delete;
};
