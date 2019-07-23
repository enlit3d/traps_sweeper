#include "text_sprite.h"

#include "glyph_texture_sheet.h"
#include "src/engine/render/texture/texture.h"
#include "src/ext/math.h"
#include "glyph.h"

#include <array>
#include <iostream>

#include "utf.h"
#include <cctype>

namespace engine::render {
	using UTF8Stream = ext::utf::UTF8Stream;

	struct TextLineIter {
		UTF8Stream operator*() {
			auto seg = UTF8Stream{ _text, _glyphIndex, _text.count() };
			auto endIndex = _nextIndex;
			auto w = 0;
			auto prevGroupLength = 0;
			for (auto c : seg) {
				if (_settings.maxLineWidth > 0) {
					auto& glyph = _sheet.get(c, _settings.fontId, _settings.fontSize);
					w += glyph.metrics.advanceX;

					if (ext::utf::numOctets(c[0]) == 1 && std::isspace(c[0])) { //TODO: maybe include locale differences as well
						prevGroupLength = 0;
					}
					else {
						prevGroupLength++;
					}

					if (w > _settings.maxLineWidth && endIndex > _glyphIndex) {
						if (prevGroupLength == 0) {
							// right at a whitespace
							_nextIndex++; // skip it all together
						} else if (prevGroupLength <= 16 && prevGroupLength < (endIndex - _glyphIndex)) {
							auto adj = prevGroupLength;
							endIndex -= adj;
							_nextIndex -= adj-1;
						}
						break;
					}
				}
				_nextIndex++;
				if (c[0] == '\n' && c[1] == '\0') {
					break;
				}
				endIndex++;
			}
			return UTF8Stream{_text, _glyphIndex, endIndex };
		}
		bool operator!=(const TextLineIter& other) const {
			return _glyphIndex != other._glyphIndex;
		}
		TextLineIter& operator++() {
			_glyphIndex = _nextIndex;
			return *this;
		}

		GlyphTextureSheet& _sheet;
		UTF8Stream& _text;
		TextSprite::TextSettings& _settings;
		i32 _glyphIndex;
		i32 _nextIndex;
	};

	struct SplitLines {
		TextLineIter begin() {
			return TextLineIter{ _sheet, _text, _settings, 0 };
		}

		TextLineIter end() {
			return TextLineIter{ _sheet, _text, _settings, _text.count() };
		}

		GlyphTextureSheet& _sheet;
		UTF8Stream& _text;
		TextSprite::TextSettings& _settings;
	};

	void TextSprite::setText(str_ref text)
	{
		auto utfs = ext::utf::UTF8Stream(text.c_str());
		auto count = utfs.count();
		auto vCount = count * 4;
		auto iCount = count * 6;

		if (this->_meshes.size() == 0 || this->_meshes[0].dataPtr.vCount() < vCount) {
			this->_meshes.clear();
			auto mat = Material{};
			mat.modelData = mat::MODEL_DATA_USE;
			mat.textureInput(_glyphSheet.texture());
			this->attachCustom(vCount, iCount, mat);
		}

		ChunkPointer& ptr = this->_meshes[0].dataPtr;
		this->_meshes[0].matInst.setColor(this->settings.color, Usage::COLOR_MULT);

		auto& fontMetrices = _glyphSheet.getFontMetrics(this->settings.fontId, this->settings.fontSize);
		auto posBuffer = ptr.getTempWriteBuffer<f32>("a_pos");
		auto indicesBuffer = ptr.getTempWriteIndexBuffer<u16>();
		auto uvBuffer = ptr.getTempWriteBuffer<f32>("a_uv");

		this->info.w = 0;
		this->info.h = 0;
		for (auto line : SplitLines{ _glyphSheet, utfs, this->settings }) { // first pass to calculate the w of h of the text sprite
			auto lineWidth = 0;
			for (auto c : line) {
				auto& glyph = _glyphSheet.get(c, this->settings.fontId, this->settings.fontSize);
				auto& metrics = glyph.metrics;
				lineWidth += metrics.advanceX;
			}

			this->info.w = math::max(this->info.w, lineWidth);
			this->info.h += fontMetrices.height;
		}

		auto index = 0;
		auto xShift = math::round(-this->settings.alignment.x * (f32)this->info.w);
		auto yShift = math::round(+this->settings.alignment.y * (f32)this->info.h) - (f32)fontMetrices.maxY;

		auto maxUsedW = 0.0f;
		auto maxUsedH = 0.0f;
		for (auto line : SplitLines{ _glyphSheet, utfs, this->settings }) {
			auto dx = xShift;
			for (auto c : line) {
				auto& glyph = _glyphSheet.get(c, this->settings.fontId, this->settings.fontSize);
				auto& textureAtlas = glyph.texture;
				auto& metrics = glyph.metrics;
				f32 w = (f32)(textureAtlas.w());
				f32 h = (f32)(textureAtlas.h());
				f32 x = dx + metrics.offsetX;
				f32 y = yShift + metrics.offsetY;
				{
					auto i = index * 4 * 3;
					posBuffer[i++] = w + x;			posBuffer[i++] = h + y;		posBuffer[i++] = 0.0f;
					posBuffer[i++] = 0.0f + x;		posBuffer[i++] = h + y;		posBuffer[i++] = 0.0f;
					posBuffer[i++] = 0.0f + x;		posBuffer[i++] = 0.0f + y;		posBuffer[i++] = 0.0f;
					posBuffer[i++] = w + x;			posBuffer[i++] = 0.0f + y; 	posBuffer[i++] = 0.0f;
					dx += metrics.advanceX;
				}

				{
					const auto uvs = textureAtlas.uvs();
					const auto bias = -0.00001f;
					auto i = index * 4 * 2;
					uvBuffer[i++] = uvs.u1 + bias;		uvBuffer[i++] = uvs.v0 + bias;
					uvBuffer[i++] = uvs.u0 + bias;		uvBuffer[i++] = uvs.v0 + bias;
					uvBuffer[i++] = uvs.u0 + bias;		uvBuffer[i++] = uvs.v1 + bias;
					uvBuffer[i++] = uvs.u1 + bias;		uvBuffer[i++] = uvs.v1 + bias;
				}


				{
					auto s = index * 4 + ptr.vOffset();
					auto i = index * 6;
					indicesBuffer[i++] = 0 + s; indicesBuffer[i++] = 1 + s;	indicesBuffer[i++] = 2 + s;
					indicesBuffer[i++] = 0 + s; indicesBuffer[i++] = 2 + s;	indicesBuffer[i++] = 3 + s;
				}

				++index;
			}

			yShift -= (f32)fontMetrices.height;
			dx = 0.0f;
		}

		// "delete" unused buffer data
		for (auto i = index*6; i < indicesBuffer.count(); ++i) {
			indicesBuffer[i] = 0;
		}
	}
}