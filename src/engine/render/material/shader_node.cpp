#include "shader_node.h"
#include <cctype>

namespace engine::render
{
	ShaderVar::ShaderVar(str_ref name, str_ref glType, Usage usage)
		: name(name),
		usage(usage),
		glType(glType)
	{}
	bool ShaderVar::operator==(const ShaderVar & other) const
	{
		return this->name == other.name;
	}
	ShaderNode::ShaderNode(ShaderVar output)
	{
		outputs.push_back(output);
	}

	ShaderNode& ShaderNode::makeChildNode(ShaderVar output)
	{
		return inputNodes.emplace_back(output);
	}

	ShaderNode& ShaderNode::inputVar(str_ref name, str_ref glType, DataType type)
	{
		auto& v = inputs.emplace_back(name, glType, ShaderVar::ATTRIB);
		v.dataType = type;
		return *this;
	}

	ShaderNode& ShaderNode::inputUnif(str_ref name, str_ref glType)
	{
		inputs.emplace_back(name, glType, ShaderVar::UNIFORM);
		return *this;
	}

	ShaderNode& ShaderNode::inputNode(ShaderNode node)
	{
		inputNodes.push_back(node);
		return *this;
	}



}