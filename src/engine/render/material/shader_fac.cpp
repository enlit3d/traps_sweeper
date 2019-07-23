#include "shader_fac.h"

#include "material.h"
#include "shader_node.h"
#include "usage_defines.h"
#include "extern/fmt/include/fmt/format.h"


namespace engine::render {
	str_t usageToStr(Usage usage) {
		return std::to_string(static_cast<std::underlying_type_t<Usage>>(usage));
	}

	ShaderGen::ShaderResult generate(const MatFlags& matFlags) {

		auto vertexOutput = ShaderNode{ {"gl_Position", "vec4"} };
		vertexOutput.inputVar("view_pos", "vec4");
		vertexOutput.source = "gl_Position = view_pos;";

		// camera
		auto& camNode = vertexOutput.makeChildNode({ "view_pos", "vec4" });
		camNode.inputVar("model_pos", "vec4");
		camNode.inputUnif("u_view", "mat4");
		camNode.source = "vec4 view_pos = u_view * model_pos;";

		if (matFlags.transform == mat::TRANSFORM_STD) {
			// use model transforms

			auto& modelNode = camNode.makeChildNode( {"model_pos", "vec4"} );
			modelNode.inputVar("a_pos", "vec3");
			modelNode.inputVar("a_mIndex", "float");
			modelNode.inputUnif("u_transforms", "sampler2D");
			modelNode.inputUnif("u_transformsInvWH", "vec2");
			modelNode.setup =
R"(mat4 read_matrix(float modelIndex, sampler2D texture, vec2 invTextureWH){
	float pixelPerIndex = 4.0;
	float row = floor(modelIndex * pixelPerIndex *invTextureWH.x);
	vec2 uv = vec2(
		(modelIndex * pixelPerIndex + 0.5) * invTextureWH.x - row, 
		(row + 0.5)*invTextureWH.y
	);
	vec2 shift = vec2(invTextureWH.x, 0.0);

	vec4 first = texture2D(texture, uv);
	vec4 second = texture2D(texture, uv + shift);
	vec4 third = texture2D(texture, uv + shift*2.0);
	vec4 fourth = texture2D(texture, uv + shift*3.0);
	return mat4(first, second, third, fourth);	
};)";
			modelNode.source =
R"(mat4 u_model = read_matrix(a_mIndex, u_transforms, u_transformsInvWH);
vec4 model_pos = u_model * vec4(a_pos, 1.0);)";

		}
		else if (matFlags.transform == mat::TRANSFORM_POS_ONLY) {
			// use pos-only transforms

			auto& modelNode = camNode.makeChildNode({ "model_pos", "vec4" });
			modelNode.inputVar("a_pos", "vec3");
			modelNode.inputVar("a_mIndex", "float");
			modelNode.inputUnif("u_translations", "sampler2D");
			modelNode.inputUnif("u_translationsInvWH", "vec2");
			modelNode.setup = R"(
vec4 read_vec4(float slotIndex, sampler2D texture, vec2 invTextureWH){
	float pixelPerIndex = 1.0;
	float row = floor(slotIndex * pixelPerIndex *invTextureWH.x);
	vec2 uv = vec2(
		(slotIndex * pixelPerIndex + 0.5) * invTextureWH.x - row, 
		(row + 0.5)*invTextureWH.y
	);
	return vec4(texture2D(texture, uv));
};)";
			modelNode.source =
				R"(vec4 model_pos = vec4(a_pos, 1.0) + vec4(read_vec4(a_mIndex, u_translations, u_translationsInvWH).xyz, 0.0);)";

		}

		auto fragOutput = ShaderNode{ {"gl_FragColor", "vec4"} };
		fragOutput.inputVar("outColor", "vec4");
		fragOutput.source = "gl_FragColor = outColor;";

		auto fragData = ShaderNode{ {"fragData", "FragData"} };
		fragData.setup =
R"(
struct FragData {
	vec4 colorBase;
	vec4 colorMask;
};
)";
		fragData.source = "FragData fragData = FragData(vec4(0.0, 0.0, 0.0, 0.0), vec4(1.0, 1.0, 1.0, 1.0));";

		auto noLighting = ShaderNode{ {"outColor", "vec4"} };
		noLighting.inputNode(fragData);
		noLighting.inputVar("fragData", "FragData");
		noLighting.source = "vec4 outColor = fragData.colorBase * fragData.colorMask;";

		if (matFlags.modelData == mat::MODEL_DATA_USE) {
			auto colorNode = ShaderNode({ "fragData", "FragData" });
			colorNode.inputVar("a_matSlot", "float");
			colorNode.inputUnif("u_matData", "sampler2D");
			colorNode.inputUnif("u_matDataInvWH", "vec2");
			colorNode.setup =
R"(
vec4 read_vec4(float slotIndex, sampler2D texture, vec2 invTextureWH){
	float pixelPerIndex = 1.0;
	float row = floor(slotIndex * pixelPerIndex *invTextureWH.x);
	vec2 uv = vec2(
		(slotIndex * pixelPerIndex + 0.5) * invTextureWH.x - row, 
		(row + 0.5)*invTextureWH.y
	);
	return texture2D(texture, uv);
};)";
			using namespace fmt::literals;
			
			colorNode.source = 
			
R"(
	vec4 info = read_vec4(a_matSlot, u_matData, u_matDataInvWH);
	vec4 rgba = read_vec4(a_matSlot+1.0, u_matData, u_matDataInvWH);
	fragData.colorBase += rgba * (int(info.r*255.1) == {COLOR_ADD} ? 1.0 : 0.0);
	fragData.colorMask *= mix(vec4(1.0), rgba, (int(info.r*255.1) == {COLOR_MULT} ? 1.0: 0.0));
)"_format(
	"COLOR_ADD"_a=usageToStr(Usage::COLOR_ADD),
	"COLOR_MULT"_a=usageToStr(Usage::COLOR_MULT)
);

			noLighting.inputNode(colorNode);
		}

		if (matFlags.textures == mat::TEXTURE_ONE) {
			auto node = ShaderNode({ "fragData", "FragData" });
			node.inputVar("a_uv", "vec2");
			node.inputUnif("u_texture0", "sampler2D");

			node.source = "fragData.colorBase += texture2D(u_texture0, a_uv);";

			noLighting.inputNode(node);
		}

		if (matFlags.modelData == mat::MODEL_DATA_NONE && matFlags.textures == mat::TEXTURE_NONE) {
			auto colorNode = ShaderNode({ "fragData", "FragData" });
			colorNode.source = "fragData.colorBase = vec4(1.0, 1.0, 0.0, 1.0);";

			noLighting.inputNode(colorNode);
		}

		fragOutput.inputNode(noLighting);

		return render::ShaderGen::generateShader(vertexOutput, fragOutput);

	}


	ShaderGen::ShaderResult& ShaderFac::get(const MatFlags& matFlags)
	{
		if (_generated.count(matFlags) == 0) {
			_generated[matFlags] = generate(matFlags);
		}

		return _generated[matFlags];
	}
}