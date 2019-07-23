#include "shader_gen.h"
#include "src/engine/utils/string.h"

namespace engine::render {
	/*

	SHADER GENERATIONS

	*/

	struct ShaderGenHelpers {
		static void replaceVars(str_t& str, str_ref from, str_ref to) {
			size_t start_pos = 0;
			while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
				auto lookAhead = start_pos + from.length();
				if (lookAhead < str.length() && isalnum(static_cast<unsigned char>(str[lookAhead]))) {
					start_pos += from.length();
				}
				else if (start_pos > 0 && isalnum(static_cast<unsigned char>(str[start_pos - 1]))) {
					start_pos += from.length();
				}
				else {
					str.replace(start_pos, from.length(), to);
					start_pos += to.length(); // handles case where 'to' is a substring of 'from'
				}

			}
		}
	};

	struct ShaderGenerator {
		using Lines = std::vector<str_t>;
		using VarList = std::vector<ShaderVar>;

		struct Results {
			VarList inputs = {};
			VarList outputs = {};
			Lines setupLines = {};
			Lines lines = {};
		};

		enum class SHADER { VERTEX, FRAGMENT };

		void processNode(const ShaderNode& node, SHADER shader = SHADER::VERTEX) {
			auto& r = shader == SHADER::VERTEX ? vStuff : fStuff;
			str_t source = node.source; // create a copy (so it can be modified)

			for (auto& var : node.inputs) {
				if (var.usage == ShaderVar::ATTRIB || var.usage == ShaderVar::UNIFORM || var.usage == ShaderVar::VARYING) {
					str_t tag = "";
					str_t name = var.name;
					ShaderVar::Usage usage = ShaderVar::ATTRIB;
					if (var.usage == ShaderVar::UNIFORM) {
						if (!string::startsWith(name, "u_")) {
							name = "u_" + name;
							ShaderGenHelpers::replaceVars(source, var.name, name);
						}
						usage = ShaderVar::UNIFORM;
					}
					else {
						if (shader == SHADER::VERTEX) {
							if (!string::startsWith(name, "a_")) {
								name = "a_" + name;
								ShaderGenHelpers::replaceVars(source, var.name, name);
							}
							usage = ShaderVar::ATTRIB;
						}
						else if (shader == SHADER::FRAGMENT) {
							auto passThroughVarying = true;
							for (auto& vOutput : vStuff.outputs) {
								if (vOutput == var) {
									passThroughVarying = false;
									break;
								}
							}
							if (!string::startsWith(name, "v_")) {
								name = "v_" + name;
								ShaderGenHelpers::replaceVars(source, var.name, name);
							}
							if (passThroughVarying) {
								auto attrName = !string::startsWith(var.name, "a_") ? "a_" + var.name : var.name;
								vStuff.inputs.emplace_back(attrName, var.glType, ShaderVar::ATTRIB);
								vStuff.lines.push_back(name + "= " + attrName + ";");
								vStuff.outputs.emplace_back(name, var.glType, ShaderVar::VARYING);
							}
							usage = ShaderVar::VARYING;
						}
					}
					r.inputs.emplace_back(name, var.glType, usage);
				}
			}

			for (auto& var : node.outputs) {
				if (var.name == "gl_Position" || var.name == "gl_FragColor") { continue; }

				if (shader == SHADER::VERTEX && (var.usage == ShaderVar::VARYING || var.usage == ShaderVar::ATTRIB)) {
					r.outputs.emplace_back(var.name, var.glType, ShaderVar::VARYING);
				}
				//else if (var.usage == ShaderVar::INTERNAL) {
					//r.lines.push_back(var.glType + " " + var.name + ";");
				//}
			}

			r.lines.push_back(source);
			if (!node.setup.empty()) {
				r.setupLines.push_back(node.setup);
			}
		}
		void processNodes(ShaderNode& node, SHADER shader) {
			for (auto& childNode : node.inputNodes) {
				for (auto& out : childNode.outputs) {
					for (auto& in : node.inputs) {
						if (out == in) {
							in.usage = ShaderVar::INTERNAL;
							out.usage = ShaderVar::INTERNAL;
						}
					}
				}
				processNodes(childNode, shader);
			}

			processNode(node, shader);
		};
		str_t compileSrc(Results& results) {
			Lines vSrc = {};

			for (auto& in : results.inputs) {
				str_t tag = "";
				switch (in.usage) {
				case ShaderVar::ATTRIB:
					tag = "attribute ";
					break;
				case ShaderVar::UNIFORM:
					tag = "uniform ";
					break;
				case ShaderVar::VARYING:
					tag = "varying ";
					break;
				default:
					throw;
				}
				vSrc.push_back(tag + in.glType + " " + in.name + ";");
			}

			for (auto& out : results.outputs) {
				str_t tag = "";
				switch (out.usage) {
				case ShaderVar::VARYING:
					tag = "varying ";
					break;
				default:
					throw;
				}
				vSrc.push_back(tag + out.glType + " " + out.name + ";");
			}

			for (auto& line : results.setupLines) {
				vSrc.push_back(line);
			}
			vSrc.push_back("void main(){");
			for (auto& line : results.lines) {
				vSrc.push_back(line);
			}
			vSrc.push_back("}");

			auto r = string::join(vSrc, "\n");

			return r;
		};

		static i32 glTypeToElemsPerVertex(str_ref glType) {
			if (glType == "vec4") {
				return 4;
			}
			else if (glType == "vec3") {
				return 3;
			}
			else if (glType == "vec2") {
				return 2;
			}
			else if (glType == "float") {
				return 1;
			}
			else {
				throw;
			}
		}

		void genLayoutHelper(Layout& layout, Results& results) {
			for (auto& v : results.inputs) {
				if (v.usage != v.ATTRIB) { continue; }
				layout.attrPointer(v.name, v.dataType, glTypeToElemsPerVertex(v.glType));
			}
		};

		Layout genLayout(Results& a, Results& b) {
			auto l = Layout{};
			genLayoutHelper(l, a);
			genLayoutHelper(l, b);
			return l;
		}

		ShaderGen::ShaderResult generate(ShaderNode& vertexNode, ShaderNode& fragNode) {
			processNodes(vertexNode, SHADER::VERTEX);
			processNodes(fragNode, SHADER::FRAGMENT);
			auto r = ShaderGen::ShaderResult {
				opengl::GPUShader{compileSrc(vStuff), compileSrc(fStuff)},
				genLayout(vStuff, fStuff)
			};
			return r;
		}

	public:
		Results vStuff = {};
		Results fStuff = {};
	};

	ShaderGen::ShaderResult ShaderGen::generateShader(ShaderNode & vertexNode, ShaderNode& fragNode)
	{
		return ShaderGenerator{}.generate(vertexNode, fragNode);
	}
}