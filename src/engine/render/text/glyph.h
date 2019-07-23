#pragma once
#include "../texture/texture.h"
namespace engine::render {
	struct GlyphMetrics {
		i32 offsetX = 0;
		i32 offsetY = 0;
		i32 advanceX = 0;
	};

	struct FontMetrics {
		i32 height = 0;
		i32 minY = 0;
		i32 maxY = 0;
	};

	struct Glyph {
		Texture texture;
		GlyphMetrics metrics;
	};
}