#pragma once

#include "src/basic_types.h"
#include "../opengl/defines/gl_types.h"
#include "src/string_type.h"
#include <vector>

namespace engine::render
{
	class ShaderVar {
	public:
		enum Usage {INTERNAL, ATTRIB, UNIFORM, VARYING};
		ShaderVar(str_ref name, str_ref glType, Usage usage=ATTRIB);
		bool operator==(const ShaderVar& other) const;
	public:
		str_t name;
		Usage usage;
		str_t glType;
		DataType dataType = DataType::F32;
	};

	class ShaderNode {
	public:
		ShaderNode(ShaderVar output);
		ShaderNode& makeChildNode(ShaderVar output); // returns reference to newly created node
		ShaderNode& inputVar(str_ref name, str_ref glType, DataType type = DataType::F32); // returns self
		ShaderNode& inputUnif(str_ref name, str_ref glType);
		ShaderNode& inputNode(ShaderNode node); // return self, note that it takes inputNode by copy
	public:
		std::vector<ShaderVar> outputs = {};
		std::vector<ShaderVar> inputs = {};
		std::vector<ShaderNode> inputNodes = {};
		str_t source = "";
		str_t setup = "";
	};
}