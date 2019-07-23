#pragma once

#include "src/basic_types.h"
#include "src/string_type.h"
#include <memory>

namespace engine::render {

	class Texture;
	struct Glyph;
	struct FontMetrics;

	class GlyphTextureSheet {
	public:
		using FontId = i32;
		using Letter = const char*;
	public:
		GlyphTextureSheet();
		~GlyphTextureSheet();

		FontId loadFont(str_ref filePath);
		const Glyph& get(Letter letter, FontId fontId, i32 pointSize=32);
		const FontMetrics& getFontMetrics(FontId fontId, i32 pointSize=32) const;
		Texture& texture();
	private:
		struct Impl;
		std::unique_ptr<Impl> _impl;
	};
}