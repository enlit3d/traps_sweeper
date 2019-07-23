#pragma once
#include "impl/data_texture.h"
#include "impl/buffered_data_texture.h"
#include "transforms.h"

namespace engine::render {
	namespace detail {
		//using MatTexture = ManagedTexture<BufferedDataTexture<u8, 4, 2>>;
		using MatTexture = ManagedTexture<DataTexture<u8, 4>>;
		using MatTextureHandle = MatTexture::UniqueHandle;

		//using TransformsTexture = ManagedTexture<BufferedDataTexture<f32, 16, 2>>;
		using TransformsTexture = ManagedTexture<DataTexture<f32, 16>>;
		using TransformsHandle = TransformsTexture::UniqueHandle;

		//using PosTexture = ManagedTexture<BufferedDataTexture<f32, 4, 2>>;
		using PosTexture = ManagedTexture<DataTexture<f32, 4>>;
		using PosHandle = PosTexture::UniqueHandle;

		class TransformsProxy : public Transforms {
		public:
			void write(memory::ArrayView<f32>& buffer);
		};
	}
}