#pragma once
#include "src/basic_types.h"
#include "src/engine/render/material/material.h"
#include "src/engine/render/chunk/attr_chunk.h"
//#include "model.h"
#include <memory>
#include "model.h"
namespace engine::render
{
	class RenderContext;
	class Transforms;
	class BasicCamera;

	namespace opengl {
		class GPUTexture;
		class GPUShader;
	}

	class DrawBatch;
	class DrawBatch : NotCopyable {
	public:
		DrawBatch(RenderContext& ctx);
		~DrawBatch();
		DrawBatch(DrawBatch&&) noexcept;
		DrawBatch& operator=(DrawBatch&&) noexcept;
		RenderContext& ctx();

	public:
		i32 drawAll(BasicCamera& camera);
		void upload();
		// returns number of Models in the batch
		i32 count() const;
	private:
		friend class Model;
		detail::MeshHandles create(const Material& mat, i32 vCount, i32 iCount);
		//ChunkPointer getPtr(const Material& mat, i32 vertexCount, i32 indexCount);
		//ChunkPointer getPtr(const Layout& layout, i32 vertexCount, i32 indexCount);
		detail::ModelHandle getModelHandle(mat::Flag<mat::Tag::TRANSFORM> transform);
		opengl::GPUShader& getShaderAndApplyBatchSettings(MatInst& matInst);
		static void applyMatInstSettings(MatInst& matInst, opengl::GPUShader& shader);

	private:
		struct Impl;
		std::unique_ptr<Impl> _impl;
	};

	static_assert(std::is_nothrow_move_assignable_v<DrawBatch>);
}