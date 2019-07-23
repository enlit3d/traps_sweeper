#pragma once

#include "src/basic_types.h"
#include "src/string_type.h"
#include <memory>

namespace engine::render {
	class AttrChunk;
}

namespace engine::render::opengl {
	class GPUTexture;
}

namespace engine::render::opengl
{
	class GPUShader : NotCopyable {
	public:
		GPUShader();
		GPUShader(str_ref vertexSrc, str_ref fragSrc);
		GPUShader(GPUShader&&) noexcept;
		GPUShader& operator=(GPUShader&&) noexcept;
		~GPUShader();
		void use();
		bool hasUniform(str_ref name) const;
		void setUniform(str_ref name, const f32* data);
		void setUniform(str_ref name, GPUTexture& texture, i32 slot);
		bool hasAttrib(str_ref name) const;
		void setAttribs(const AttrChunk& chunk);
	private:
		struct Impl;
		std::unique_ptr<Impl> _impl;
	};
}