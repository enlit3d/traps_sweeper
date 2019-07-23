#include "draw_batch.h"
#include "../material/material.h"
#include "../material/shader_fac.h"
#include "../ctx/render_context.h"
#include "../chunk/attr_chunk.h"
#include "impl/data_texture.h"
#include <vector>

#include "transforms.h"
#include "src/engine/render/opengl/gl_shader.h"
#include "src/engine/render/api/draw.h"
#include "mat_texture.h"
#include "../scene/camera.h"

#include "src/engine/utils/console.h"

namespace engine::render
{

	class TransformsTextureWrapper {
		using Texture_T = Variant<detail::TransformsTexture, detail::PosTexture>;
		using Handle_T = detail::ModelHandle;
	public:
		TransformsTextureWrapper(mat::Flag<mat::Tag::TRANSFORM> transform) 
			: _type{transform}
		{
			if (transform == mat::TRANSFORM_STD) {
				_data = detail::TransformsTexture{};
			}
			else if (transform == mat::TRANSFORM_POS_ONLY) {
				_data = detail::PosTexture{};
			}
			else {
				throw;
			}
		}

		bool isFor(mat::Flag<mat::Tag::TRANSFORM> transform) const {
			return _type == transform;
		}

		void upload() {
			//std::visit([](auto&& v) {v.upload(); }, _data); return;
			callHelper<void>([](auto&& dataTexture) {
				return dataTexture.upload();
			});
		}
		Handle_T getHandle(){
			//return std::visit([](auto&& v) -> Handle_T {return v.acquire(); }, _data);
			return callHelper<Handle_T>([](auto&& dataTexture) -> Handle_T {
				return dataTexture.acquire();
			});
		}

		opengl::GPUTexture& texture() {
			//return std::visit([](auto&& v) -> opengl::GPUTexture & {return v.texture(); }, _data);
			return callHelper<opengl::GPUTexture&>([](auto&& dataTexture) -> opengl::GPUTexture& {
				return dataTexture.texture();
			});
		}

		i32 count() {
			//return std::visit([](auto&& v) -> i32 {return v.count(); }, _data);
			return callHelper<i32>([](auto&& dataTexture) -> i32 {
				return dataTexture.count();
			});
		}

	private:
		template <typename Ret, typename Fn> // theres probably some way to infer the Return Type of Fn, but I'm not smart enough to figure it out
		Ret callHelper(Fn fn){
			auto index = _data.index();
			switch (index) {
				case ext::variant_index<Texture_T, detail::TransformsTexture>() :
					return fn(std::get<detail::TransformsTexture>(_data));
				case ext::variant_index<Texture_T, detail::PosTexture>() :
					return fn(std::get<detail::PosTexture>(_data));
				default:
					throw;
			}
		}

	private:
		Texture_T _data = {};
		mat::Flag<mat::Tag::TRANSFORM> _type = {};
	};

	struct RenderChunk {
		Material material = Material{};
		std::unique_ptr<AttrChunk> chunkRef = {};
		//i32 transformTextureIndex = -1;
	};

	struct ShaderGroup {
		ShaderGroup(RenderContext& ctx, MatFlags matFlags, Layout layout) :
			matFlags{ matFlags },
			layout{ layout },
			matTexture{}
		{}
		MatFlags matFlags = {};
		Layout layout = {};
		std::vector<RenderChunk> chunks = {};
		detail::MatTexture matTexture;
	};

	struct DrawBatch::Impl {
		Impl(RenderContext& ctx)
			: ctx(ctx)
		{}

		//static constexpr auto MAX_MODELS_PER_TEXTURE = 65536;

	public:

		detail::MeshHandles attachModel(const Material& mat, i32 vCount, i32 iCount) {
			auto& group = getGroup(mat);
			return placeIntoGroup(group, mat, vCount, iCount);
		}

	private:

		ShaderGroup& getGroup(const MatFlags& mat) {
			ShaderGroup* p = nullptr;
			for (auto& g : groups) {
				if (g.matFlags == mat) {
					p = &g;
				}
			}

			if (p == nullptr) {
				// not found, need to alloc
				auto& lo = ctx.shaderFac().get(mat).layout;

				groups.emplace_back(this->ctx, mat, lo);
				p = &(groups.back());
			}

			return *p;
		}
		detail::MeshHandles placeIntoGroup(ShaderGroup& group, Material mat, i32 vCount, i32 iCount) {
			RenderChunk* foundChunk = nullptr;
			for (auto& b : group.chunks) {  
				if (b.chunkRef->canFit(vCount, iCount)
					//&& transformTextures.at(b.transformTextureIndex).count() < MAX_MODELS_PER_TEXTURE
				) { 
					foundChunk = &b;
					break;
				}
			}
			if (!foundChunk) {
				// failed to alloc from existing chunks...
				auto& batch = group.chunks.emplace_back(RenderChunk{ mat, std::make_unique<AttrChunk>() });
				batch.chunkRef->init(group.layout);
				foundChunk = &batch;
			}
			if (!foundChunk) { throw; } // outta memory??!

			auto& chunk = *foundChunk;

			auto matInst = MatInst{mat};
			if (mat.modelData == mat::MODEL_DATA_USE) {
				matInst.bindHandle(group.matTexture.acquire(2));
			}

			return {
				std::move(matInst),
				chunk.chunkRef->request(vCount, iCount)
			};

		}

		/*
		i32 getTransformsTextureIndex(mat::Flag<mat::Tag::TRANSFORM> transform) {
			auto index = 0;
			for (auto& t : transformTextures) {
				if (t.isFor(transform) && t.count() < MAX_MODELS_PER_TEXTURE) {
					return index;
				}
				++index;
			}

			// reached here, that means a suitable texture isn't found
			transformTextures.emplace_back(transform);
			return (i32)transformTextures.size();
		}
		*/

	public:
		RenderContext& ctx;
		std::vector<ShaderGroup> groups = {};
		//std::vector<TransformsTextureWrapper> transformTextures = {};
		detail::TransformsTexture transforms = {};
		detail::PosTexture translations = {};
	};

	DrawBatch::DrawBatch(RenderContext & ctx)
		:_impl(std::make_unique<Impl>(ctx))
	{
	}

	DrawBatch::~DrawBatch() = default;

	DrawBatch::DrawBatch(DrawBatch&&) noexcept = default;

	DrawBatch& DrawBatch::operator=(DrawBatch&&) noexcept = default;

	RenderContext& DrawBatch::ctx()
	{
		return _impl->ctx;
	}

	i32 DrawBatch::drawAll(BasicCamera& camera)
	{

		auto drawn = 0;

		for (auto& group : _impl->groups) {
			auto& shader = ctx().shaderFac().get(group.matFlags).shader;

			shader.setUniform("u_view", camera.viewProj().data());

			if (shader.hasUniform("u_transforms")) {
				auto& t = _impl->transforms.texture();
				f32 invWH[2] = { 1.0f / t.w(), 1.0f / t.h() };
				shader.setUniform("u_transforms", t, 1);
				shader.setUniform("u_transformsInvWH", invWH);
			}
			else if (shader.hasUniform("u_translations")) {
				auto& t = _impl->translations.texture();
				f32 invWH[2] = { 1.0f / t.w(), 1.0f / t.h() };
				shader.setUniform("u_translations", t, 1);
				shader.setUniform("u_translationsInvWH", invWH);
			}


			if (shader.hasUniform("u_matData"))	{
				auto& t = group.matTexture.texture();
				shader.setUniform("u_matData", t, 2);
				f32 invWH[2] = { 1.0f / t.w(), 1.0f / t.h() };
				shader.setUniform("u_matDataInvWH", invWH);
			}

			for (auto& b : group.chunks) {
				auto& chunk = *b.chunkRef;
				if (shader.hasUniform("u_texture0")) {
					shader.setUniform("u_texture0", b.material._textures[0].texture.src(), 3);
				}
				drawn += render::drawWithoutUpload(chunk, shader);
			}
		}

		return drawn;
	}

	void DrawBatch::upload()
	{

		_impl->transforms.upload();
		_impl->translations.upload();

		for (auto& group : _impl->groups) {
			for (auto& b : group.chunks) {
				auto& chunk = *b.chunkRef;
				chunk.upload();
			}
			group.matTexture.upload();
		}
	}

	i32 DrawBatch::count() const
	{
		return _impl->transforms.count() + _impl->translations.count();
	}

	detail::MeshHandles DrawBatch::create(const Material& mat, i32 vCount, i32 iCount)
	{
		return _impl->attachModel(mat, vCount, iCount);
	}
	/*
	ChunkPointer DrawBatch::getPtr(const Material& mat, i32 vCount, i32 iCount)
	{
		return _impl->getPtrFromGroup(_impl->getGroup(mat), vCount, iCount);
	}
	*/

	detail::ModelHandle DrawBatch::getModelHandle(mat::Flag<mat::Tag::TRANSFORM> transform)
	{
		if (transform == mat::TRANSFORM_STD) {
			return detail::ModelHandle{ _impl->transforms.acquire() };
		}
		else if (transform == mat::TRANSFORM_POS_ONLY) {
			return detail::ModelHandle{ _impl->translations.acquire() };
		} else {
			throw;
		}
	}
	opengl::GPUShader& DrawBatch::getShaderAndApplyBatchSettings(MatInst& matInst)
	{
		auto& shader = ctx().shaderFac().get(matInst.matFlags()).shader;

		if (shader.hasUniform("u_transforms")) {
			auto& t = _impl->transforms.texture();
			f32 invWH[2] = { 1.0f / t.w(), 1.0f / t.h() };
			shader.setUniform("u_transforms", t, 1);
			shader.setUniform("u_transformsInvWH", invWH);
		}
		else if (shader.hasUniform("u_translations")) {
			auto& t = _impl->translations.texture();
			f32 invWH[2] = { 1.0f / t.w(), 1.0f / t.h() };
			shader.setUniform("u_translations", t, 1);
			shader.setUniform("u_translationsInvWH", invWH);
		}

		if (shader.hasUniform("u_matData")) {
			auto& t = matInst._modelData.texture();
			shader.setUniform("u_matData", t, 2);
			f32 invWH[2] = { 1.0f / t.w(), 1.0f / t.h() };
			shader.setUniform("u_matDataInvWH", invWH);
		}

		return shader;
	}
	void DrawBatch::applyMatInstSettings(MatInst& matInst, opengl::GPUShader& shader)
	{
		if (shader.hasUniform("u_texture0")) {
			shader.setUniform("u_texture0", matInst.material()._textures[0].texture.src(), 3);
		}
	}
}