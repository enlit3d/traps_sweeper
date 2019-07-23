#pragma once
#include <map>
#include "shader_gen.h"
#include "material.h"

namespace engine::render {

	class MatFlags;
	class Layout;

	namespace opengl {
		class GPUShader;
	}

	class ShaderFac {
	public:
		ShaderGen::ShaderResult& get(const MatFlags& matFlags);
	private:
		std::map<MatFlags, ShaderGen::ShaderResult> _generated = {};
	};
}