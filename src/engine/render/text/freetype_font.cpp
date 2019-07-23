#include "freetype_font.h"
#include "utf.h"

#include "ft2build.h"
#include "freetype/ftglyph.h"
#include FT_FREETYPE_H

#include <iostream>
#include <cassert>

namespace engine::render {

	struct FreeTypeFont::Impl {
		Impl(str_ref filePath, i32 pointSize) {
			if (FT_Init_FreeType(&library)) {
				throw;
			}

			auto error = FT_New_Face(
				library,
				filePath.c_str(),
				0,
				&face
			);
			if (error) {
				if (error == FT_Err_Unknown_File_Format) {
					std::cout << "Unknown file format.";
				}
				else {
					throw;
				}
			}
			
			
			if (FT_Set_Pixel_Sizes(face, 0, pointSize)) {
				throw;
			}

			auto& m = face->size->metrics;
			fontMetrics = FontMetrics{
				m.height / 64,
				m.descender / 64,
				m.ascender / 64
			};

		}
		~Impl() {
			FT_Done_Face(face);
			FT_Done_FreeType(library);
		}

		ImageData renderGlyph(u32 ch) {
			//if (FT_Load_Char(face, ch, FT_LOAD_DEFAULT)) {
			if (FT_Load_Char(face, ch, FT_LOAD_FORCE_AUTOHINT)) {
				throw; 
			}
			
			if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL)) {
				throw;
			}

			//if (FT_Glyph_To_Bitmap(face->glyph, FT_RENDER_MODE_NORMAL, 0, false)) {
			//	throw;
			//}
			
			auto& bitmap = face->glyph->bitmap;

			auto imageData = ImageData{ (i32)bitmap.width, (i32)bitmap.rows, 4 };
			auto i = 0;
			for (u32 y = 0; y < bitmap.rows; ++y) {
				for (u32 x = 0; x < bitmap.width; ++x) {
					u8 v = static_cast<u8>(bitmap.buffer[y * bitmap.pitch + x]);
					imageData.data()[i++] = 255; // r
					imageData.data()[i++] = 255; // g
					imageData.data()[i++] = 255; // b
					imageData.data()[i++] = v; // a
				}
			}
			assert(imageData.data() + i == imageData.data() + imageData.bytes());
			return imageData;
		}
	public:
		FT_Library library;
		FT_Face face;
		FontMetrics fontMetrics;
	};

	FreeTypeFont::FreeTypeFont(FreeTypeFontSrc& src, i32 pointSize)
		: FreeTypeFont(src._filePath, pointSize)
	{
	}

	FreeTypeFont::FreeTypeFont(str_ref filePath, i32 pointSize)
		: _impl(std::make_unique<Impl>(filePath, pointSize))
	{

	}
	FreeTypeFont::~FreeTypeFont() = default;

	FreeTypeFont::FreeTypeFont(FreeTypeFont&&) noexcept = default;

	FreeTypeFont& FreeTypeFont::operator=(FreeTypeFont&&) noexcept = default;

	ImageData FreeTypeFont::renderGlyph(u32 ch)
	{
		return _impl->renderGlyph(ch);
	}
	ImageData FreeTypeFont::renderGlyph(const char* ch)
	{
		return renderGlyph(ext::utf::utf8ToUtf32(ch));
	}
	GlyphMetrics FreeTypeFont::getMetricsForPreviousRendered()
	{
		auto& slot = _impl->face->glyph;
		return {
			slot->bitmap_left,
			-((i32)slot->bitmap.rows - slot->bitmap_top),
			slot->advance.x / 64
		};
	}
	const FontMetrics& FreeTypeFont::getFontMetrics() const
	{
		return _impl->fontMetrics;
	}
	FreeTypeFontSrc::FreeTypeFontSrc(str_t filePath)  
		: _filePath{filePath}
	{
	}
}