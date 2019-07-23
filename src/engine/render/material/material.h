#pragma once

#include "src/basic_types.h"
#include "src/engine/render/model/mat_texture.h"
#include "src/engine/render/texture/texture.h"
#include "src/engine/collections/fixed_vector.h"



namespace engine::render
{
	namespace mat {
		enum class Tag {
			TRANSFORM,
			MODEL_DATA,
			TEXTURE_COUNT,
		};

		template <Tag>
		class Flag {
		public:
			Flag(i32 v=0);
			bool operator==(const Flag& other) const;
			bool operator!=(const Flag& other) const;
			bool operator<(const Flag& other) const;
		private:
			i32 _v = 0;
		};

		extern Flag<Tag::TRANSFORM> TRANSFORM_STD;
		extern Flag<Tag::TRANSFORM> TRANSFORM_POS_ONLY;
		extern Flag<Tag::MODEL_DATA> MODEL_DATA_NONE;
		extern Flag<Tag::MODEL_DATA> MODEL_DATA_USE;
		extern Flag<Tag::TEXTURE_COUNT> TEXTURE_NONE;
		extern Flag<Tag::TEXTURE_COUNT> TEXTURE_ONE;
	}

	class MatFlags {
		using Tag = mat::Tag;
	public:
		mat::Flag<Tag::TRANSFORM> transform = mat::TRANSFORM_STD;
		mat::Flag<Tag::MODEL_DATA> modelData = mat::MODEL_DATA_NONE;
		mat::Flag<Tag::TEXTURE_COUNT> textures = mat::TEXTURE_NONE;
		bool operator<(const MatFlags& other) const;
		friend bool operator==(const MatFlags& a, const MatFlags& b);
	};

	class Material : protected MatFlags {
	public:
		explicit Material(MatFlags flags = {})
			: MatFlags(flags)
		{}

		void textureInput(const Texture& texture) {
			_textures.emplace_back(InputTextureData{ texture });
			this->textures = mat::TEXTURE_ONE;
		}

		friend bool operator==(const Material& a, const Material& b) {
			if (!(static_cast<MatFlags>(a) == static_cast<MatFlags>(b))){return false;}
			if (!(a._textures == b._textures)){return false;}
			return true;
		}

		using MatFlags::operator<;
	public:
		using MatFlags::transform;
		using MatFlags::modelData;
		using MatFlags::textures;
	protected:
		struct InputTextureData {
			Texture texture;

			friend bool operator==(const InputTextureData& a, const InputTextureData& b) {
				return a.texture.src() == b.texture.src();
			}
		};

		constexpr static auto MAX_TEXTURES_COUNT = 1;

		friend class DrawBatch;
		friend class Model;
		collections::FixedVector<InputTextureData, MAX_TEXTURES_COUNT> _textures = {};
		static_assert(std::is_copy_constructible_v<decltype(textures)>);

	};
	static_assert(std::is_copy_constructible_v<Material>);

	
}