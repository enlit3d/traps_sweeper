#pragma once
#include <memory>
#include "../opengl/gl_texture.h"

namespace engine::render {

	struct Rect {
		i32 x = 0;
		i32 y = 0;
		i32 w = 0;
		i32 h = 0;
		explicit operator bool() const {
			return w >= 0 && h >= 0;
		}
	};

	struct ImageData;

	class Texture {
	public:
		Texture();
		Texture(Texture& src, Rect rect);
	public:
		struct UVRegion;
		UVRegion uvs() const;
		void write(const ImageData& imageData);

		opengl::GPUTexture& src();
		const opengl::GPUTexture& src() const;
		opengl::GPUTexture& operator* ();
		opengl::GPUTexture* operator-> ();
		i32 w() const;
		i32 h() const;
	private:
		std::shared_ptr<opengl::GPUTexture> _src;
		Rect _region = {0, 0, -1, -1};
	};

	struct Texture::UVRegion {
		f32 u0 = 0.0f;
		f32 v0 = 0.0f;
		f32 u1 = 1.0f;
		f32 v1 = 1.0f;
	};
}