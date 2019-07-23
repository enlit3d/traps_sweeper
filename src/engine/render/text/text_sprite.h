#pragma once
#include "src/engine/render/model/model.h"
#include "src/string_type.h"
#include "src/engine/render/misc/color.h"

namespace engine::render {
	class GlyphTextureSheet;

	using FontId = i32; //TODO: possibly change this
	
	
	class TextSprite : private Model {
	public:
		TextSprite(DrawBatch& batch, GlyphTextureSheet& glyphSheet, FontId fontId=0) : 
			Model(batch), _glyphSheet(glyphSheet)
		{
			settings.fontId = fontId;
		}
		void setText(str_ref text);
		MatInst& attach(MeshData& mesh, const Material& mat) = delete;

		using Model::setTransform;
		using Model::updateTransforms;
		using Model::isVis;
		using Model::setVis;
	public:
		struct Alignment {
			f32 x = 0.0f;
			f32 y = 0.0f;
		};

		struct TextSettings {
			i32 fontSize = 32;
			i32 maxLineWidth = -1;
			FontId fontId = 0;
			Alignment alignment = {0.0f, 0.0f};
			Color color = { 255,255,255,255 };
		};

		struct TextInfo {
			i32 w = 0;
			i32 h = 0;
		};
	public:
		TextSettings settings = {};
		using Model::transforms;
		TextInfo info = {};
	private: // TEMPORARY
		GlyphTextureSheet& _glyphSheet;
	};
}