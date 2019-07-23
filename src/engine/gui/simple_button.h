#pragma once
#include "base_components/button.h"
#include "simple_renderables.h"

namespace engine::gui {
	using Color = render::Color;

	class Factory {
		template <typename T>
		using Shared = std::shared_ptr<T>;
	public:
		Factory(render::DrawBatch& batch, render::GlyphTextureSheet& glyphs, render::FontId font) :
			_batch(batch), _glyphs(glyphs), _font(font)
		{}

		struct SimpleButton {
			Shared<Button> btn;
			Shared<GuiText> label;
		};

		SimpleButton makeSimpleButton(
			std::shared_ptr<Frame>& parent,
			Rect rect,
			str_ref label,
			render::Color normalColor,
			render::Color hoveredColor,
			render::Color selectedColor,
			Button::Callback_T&& callback
		) {
			auto btn = parent->emplaceChild<Button>(
				rect,
				std::move(callback),
				std::make_unique<gui::SolidBgColor>(_batch, normalColor),
				std::make_unique<gui::SolidBgColor>(_batch, hoveredColor),
				std::make_unique<gui::SolidBgColor>(_batch, selectedColor)
			);

			auto textLabel = btn->emplaceRenderable<GuiText>(_batch, _glyphs, _font);
			textLabel->textSettings().fontSize = math::roundTo<i32>(0.7f * rect.h);
			textLabel->setText(label);
			return SimpleButton {
				btn,
				textLabel
			};
		}

	public:
		render::DrawBatch& _batch; // TODO: use a memory-safe handle instead of batch& directly!!!
		render::GlyphTextureSheet& _glyphs;
		render::FontId _font;
	};
}