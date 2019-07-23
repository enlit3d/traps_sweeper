#pragma once
#include <array>
#include "src/basic_types.h"

namespace engine::render {

	class Color {
	public:
		constexpr Color(u8 r=0, u8 g=0, u8 b=0, u8 a = 255) {
			_rgba[0] = r;
			_rgba[1] = g;
			_rgba[2] = b;
			_rgba[3] = a;
		};
		const std::array<u8, 4>& getRGBA() const {
			return _rgba;
		}
	private:
		std::array<u8, 4> _rgba = { 0, 0, 0, 255 };
	};
}