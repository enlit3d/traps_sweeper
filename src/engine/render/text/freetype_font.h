#pragma once

#include <memory>
#include "src/basic_types.h"
#include "src/string_type.h"
#include "src/engine/render/texture/image_data.h"
#include "glyph.h"

namespace engine::render {
	class FreeTypeFontSrc {
	public:
		FreeTypeFontSrc(str_t filePath);
	private:
		friend class FreeTypeFont;
		str_t _filePath;
	};

	class FreeTypeFont {
	public:
		FreeTypeFont(FreeTypeFontSrc& src, i32 pointSize = 32);
		FreeTypeFont(str_ref filePath, i32 pointSize = 32);
		~FreeTypeFont();
		FreeTypeFont(FreeTypeFont&&) noexcept;
		FreeTypeFont& operator=(FreeTypeFont&&) noexcept;
		ImageData renderGlyph(u32 ch);
		ImageData renderGlyph(const char* ch);
		GlyphMetrics getMetricsForPreviousRendered();
		const FontMetrics& getFontMetrics() const;
	private:
		struct Impl;
		std::unique_ptr<Impl> _impl;
	};

}