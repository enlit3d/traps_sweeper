#pragma once

#include "base_components/frame.h"
#include "src/engine/render/model/model.h"
#include "src/engine/render/misc/color.h"
#include "src/engine/render/text/text_sprite.h"
#include "src/engine/render/mesh/mesh_gen.h"

namespace engine::gui {
	using namespace engine;
	class SolidBgColor : public gui::Renderable {
	public:
		SolidBgColor(render::DrawBatch& batch, render::Color color) :
			_model(batch)
		{
			auto mesh = render::mesh_gen::Plane(1.0f, 1.0f);
			auto mat = render::Material{};
			mat.modelData = render::mat::MODEL_DATA_USE;
			auto& mInst = _model.attach(mesh, mat);
			mInst.setColor(color);
		}
		void setVis(bool on) override {
			_model.setVis(on);
		}
		void setBounds(gui::Rect rect) override {
			auto& t = _model.transforms;
			t.pos[0] = rect.x + rect.w / 2;
			t.pos[1] = rect.y + rect.h / 2;
			t.scl[0] = rect.w;
			t.scl[1] = rect.h;
			_model.updateTransforms();
		}
	private:
		render::Model _model;
	};

	class GuiText : public gui::Renderable {
	public:
		GuiText(render::DrawBatch& batch, render::GlyphTextureSheet& glyphSheet, render::FontId fontId) :
			_text(batch, glyphSheet, fontId)
		{
			_text.settings.fontSize = 16;
			_text.settings.alignment.x = 0.5f;
			_text.settings.alignment.y = 0.5f;
		}

		void setVis(bool on) override {
			_text.setVis(on);
		}

		void setBounds(gui::Rect rect) override {
			_bounds = rect;
			updateTransforms();
		}

		void setText(str_ref text) {
			_text.setText(text);
			updateTransforms();
		}

		render::TextSprite::TextSettings& textSettings() {
			return _text.settings;
		}
	private:
		void updateTransforms() {
			if (_text.info.w == 0) { return; }
			auto& t = _text.transforms;
			t.pos[0] = _bounds.x + _bounds.w * _text.settings.alignment.x;
			t.pos[1] = _bounds.y + _bounds.h * _text.settings.alignment.y;

			// ensure the text is no bigger than the frame (scales down if cannot fit)
			auto ratio = math::min((f32)_bounds.w / (f32)_text.info.w, (f32)_bounds.h / (f32)_text.info.h);
			auto scale = 1.0f;
			if (ratio < 1.0f) {
				scale *= ratio;
			}
			t.scl[0] = scale;
			t.scl[1] = -scale;
			_text.updateTransforms();
		}

	private:
		render::TextSprite _text;
		gui::Rect _bounds = {};
	};
}