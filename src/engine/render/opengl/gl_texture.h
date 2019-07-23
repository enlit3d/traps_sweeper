#pragma once
#include "defines/gl_types.h"
#include <memory>

namespace engine::render::opengl
{
	struct TextureParams : NotCopyable {
		enum Format { RGBA };
		enum Filter { NEAREST, LINEAR, NEAREST_MIPMAP_NEAREST, LINEAR_MIPMAP_NEAREST, NEAREST_MIPMAP_LINEAR, LINEAR_MIPMAP_LINEAR };
		enum Wrap { CLAMP_TO_EDGE, MIRRORED_REPEAT, REPEAT };

		DataType dataType = DataType::U8;
		Format format = RGBA;
		Filter minFilter = NEAREST;
		Filter magFilter = NEAREST;
		Wrap wrapS = CLAMP_TO_EDGE;
		Wrap wrapT = CLAMP_TO_EDGE;
	};

	class GPUTexture {
	public:
		GPUTexture();
		~GPUTexture();
		GPUTexture(GPUTexture&&) noexcept;
		GPUTexture& operator=(GPUTexture&&) noexcept;
		const TextureParams& params() const;
		TextureParams& params(bool);
		void use();
		void resizeAndClear(i32 w, i32 h);
		void write(const byte* bytes);
		void write(const byte* bytes, i32 x, i32 y, i32 w, i32 h);
		i32 byteSize();
		i32 w() const;
		i32 h() const;
		bool operator==(const GPUTexture& other) const;
	private:
		struct Impl;
		std::unique_ptr<Impl> _impl;
	};
}