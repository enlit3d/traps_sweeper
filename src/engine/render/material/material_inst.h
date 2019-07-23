#pragma once
#include "material.h"
#include "usage_defines.h"
#include "src/engine/render/misc/color.h"

namespace engine::render {
	class MatInst : private Material {
	public:
		explicit MatInst(const Material& mat)
			: Material(mat)
		{}

		const MatFlags& matFlags() const { //TODO: get rid of these are use composition instead of inheritance...
			return *this;
		}

		Material& material() {
			return *this;
		}

		MatInst& setColor(Color color, Usage usage=Usage::COLOR_ADD) {
			if (this->modelData == mat::MODEL_DATA_NONE) { return *this; }
			auto view = _modelData.view(true);
			auto rgba = color.getRGBA();
			view[0] = static_cast<u8>(usage);
			view[4] = rgba[0];
			view[5] = rgba[1];
			view[6] = rgba[2];
			view[7] = rgba[3];
			return *this;
		}

		void tryChangeTexture(i32 textureId, const Texture& texture) {
			if (_textures.size() <= textureId) { return; }
			//TODO: other checks... etc
			_textures[textureId].texture = texture;
		}

	private:
		friend bool operator==(const MatInst& a, const MatInst& b) {
			return false; // all material instances are unique
		}
	public:
		void bindHandle(detail::MatTextureHandle&& view) {
			_modelData = std::move(view);
		}

	private:
		friend class Model;
		friend class DrawBatch;

		detail::MatTextureHandle _modelData = {};
	};

	static_assert(!std::is_copy_assignable_v<MatInst>);
	static_assert(!std::is_copy_constructible_v<MatInst>);
}