#include "glyph_texture_sheet.h"

#include "freetype_font.h"
#include "src/engine/render/texture/texture.h"
#include "src/engine/render/texture/packer.h"
#include "src/engine/render/misc/color.h"

#include "glyph.h"

#include <unordered_map>

namespace engine::render {

	using FontId = GlyphTextureSheet::FontId;
	using Letter = GlyphTextureSheet::Letter;
	using TextureAtlas = Texture;
	
	struct CacheKey {
		FontId id;
		str_t letter;
		i32 pointSize;

		bool operator==(const CacheKey& other) const {
			return this->id == other.id && this->letter == other.letter && this->pointSize == other.pointSize;
		}
	};


	struct CacheHash {
		std::size_t operator()(const CacheKey& k) const { // TODO: find a better hash func?
			const auto primeA = 16111;
			const auto primeB = 257;
			return 
				std::hash<decltype(k.id)>{}(k.id) * primeA 
				+ std::hash<decltype(k.letter)>{}(k.letter) * primeB
				+ std::hash<decltype(k.pointSize)>{}(k.pointSize);
		}
	};

	struct GlyphTextureSheet::Impl {
		constexpr static auto TEXTURE_DIM = 1024; // TODO: temporary before texture resizing is supported
		struct FontFamily {
			FontFamily(FreeTypeFontSrc&& src) 
				: src(std::move(src))
			{}
			FreeTypeFontSrc src;
			std::unordered_map<i32, FreeTypeFont> pointSizes = {};
		};
		static_assert(std::is_move_constructible_v<FontFamily>);
		static_assert(std::is_move_assignable_v<FontFamily>);
	public:
		Impl() : 
			textureSheet(),
			packer(TEXTURE_DIM, TEXTURE_DIM),
			EMPTY_GLYPH{ TextureAtlas{textureSheet, { 0, 0, 0, 0 }}, GlyphMetrics{} }
		{
			auto& params = textureSheet->params(true);
			params.format = render::opengl::TextureParams::Format::RGBA;
			params.minFilter = params.NEAREST;
			params.magFilter = params.NEAREST;
			textureSheet->resizeAndClear(TEXTURE_DIM, TEXTURE_DIM);
		}
		const Glyph& get(CacheKey k) {
			if (cache.count(k) > 0) {
				return cache.at(k);
			}
			if (fontFamilies.count(k.id) <= 0) {
				return EMPTY_GLYPH;
			}
			
			auto& font = getFontOrCreate(k.id, k.pointSize);
			auto imageData = font.renderGlyph(k.letter.c_str());
			if (imageData.w == 0 || imageData.h == 0) {
				cache.insert_or_assign(k, Glyph{ TextureAtlas{textureSheet, { 0, 0, 0, 0 }}, font.getMetricsForPreviousRendered() });
				return cache.at(k);
			}
			auto region = packer.packRegion(imageData.w, imageData.h);
			if (!region) { return EMPTY_GLYPH; }
			if (packer.w() > textureSheet->w() || packer.h() > textureSheet->h()) {
				throw;
				//textureSheet->resizeAndClear(math::toNextPowOf2(packer.w()), math::toNextPowOf2(packer.h()));
			}
			auto t = TextureAtlas{ textureSheet, Rect{region.x, region.y, region.w, region.h } };
			t.write(imageData);
			cache.insert_or_assign(k, Glyph{t, font.getMetricsForPreviousRendered()});
			return cache.at(k);
		}
		FreeTypeFont& getFontOrCreate(FontId fontId, i32 pointSize) {
			auto& fontFamily = fontFamilies.at(fontId);
			if (fontFamily.pointSizes.count(pointSize) <= 0) {
				fontFamily.pointSizes.insert_or_assign(pointSize, FreeTypeFont{ fontFamily.src, pointSize });
			}
			return fontFamily.pointSizes.at(pointSize);
		}

	public:
		std::unordered_map<CacheKey, Glyph, CacheHash> cache = {};
		std::unordered_map<FontId, FontFamily> fontFamilies = {};
		Texture textureSheet = {};
		Packer packer = {};
		Glyph EMPTY_GLYPH = {};
		i32 counter = 0;
	};


	GlyphTextureSheet::GlyphTextureSheet()
		: _impl(std::make_unique<Impl>())
	{
	}

	GlyphTextureSheet::~GlyphTextureSheet() = default;

	Texture& GlyphTextureSheet::texture()
	{
		return _impl->textureSheet;
	}

	const Glyph& GlyphTextureSheet::get(Letter letter, FontId fontId, i32 pointSize)
	{
		return _impl->get(CacheKey{ fontId, letter, pointSize });
	}

	const FontMetrics& GlyphTextureSheet::getFontMetrics(FontId fontId, i32 pointSize) const
	{
		auto& font = _impl->getFontOrCreate(fontId, pointSize);
		return font.getFontMetrics();
	}

	FontId GlyphTextureSheet::loadFont(str_ref filePath)
	{
		auto id = _impl->counter++;
		_impl->fontFamilies.insert_or_assign(id, FreeTypeFontSrc{ filePath });
		return id;
	}
}