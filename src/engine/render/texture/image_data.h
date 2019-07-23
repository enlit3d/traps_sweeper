#pragma once 
#include "src/basic_types.h"
#include "src/engine/collections/memory/raw_buffer.h"
namespace engine::render {

	struct ImageData {
		ImageData(i32 w, i32 h, i32 bytesPerPixel)
			: pixels(u64(w* h* bytesPerPixel)), w(w), h(h), bytesPerPixel(bytesPerPixel)
		{}
		ImageData(i32 w, i32 h, i32 bytesPerPixel, byte* srcData)
			: pixels(u64(w* h* bytesPerPixel)), w(w), h(h), bytesPerPixel(bytesPerPixel)
		{
			std::copy(srcData, srcData + u64(w * h * bytesPerPixel), data());
		}

		byte* data() {
			return pixels.data();
		}
		const byte* const data() const {
			return pixels.data();
		}
		size_t bytes() const {
			return pixels.size();
		}
	public:
		memory::RawMemory pixels;
		i32 w;
		i32 h;
		i32 bytesPerPixel;

	};

	static_assert(std::is_move_constructible_v<ImageData>);
	//static_assert(std::is_copy_constructible_v<ImageData>);
}