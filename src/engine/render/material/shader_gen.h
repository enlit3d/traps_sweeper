 #pragma once
#include "src/string_type.h"
#include "shader_node.h"

#include "../opengl/gl_shader.h"
#include "../chunk/layout.h"

namespace engine::render {
	
	class ShaderGen {
	public:
		struct Strings {
			str_t vertexSrc;
			str_t fragSrc;
		};

		struct ShaderResult {
			opengl::GPUShader shader;
			Layout layout;
		};

		ShaderResult static generateShader(ShaderNode& vertexNode, ShaderNode& fragNode);

	private:

	};

}