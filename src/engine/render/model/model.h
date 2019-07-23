#pragma once
#include "src/basic_types.h"

#include <vector>
#include "src/engine/render/material/material_inst.h"
#include "mat_texture.h"

#include "src/engine/render/chunk/attr_chunk.h"
#include "src/ext/variant.h"

namespace engine::render
{
	class DrawBatch;
	class Transforms;
	class MeshData;
	class MatFlags;

	namespace opengl {
		class GPUShader;
	}

	namespace detail {
		using ModelHandle = Variant<TransformsHandle, PosHandle>;

		struct MeshHandles {
			MatInst matInst;
			ChunkPointer dataPtr;
		};
	}

	class Model : NotCopyable {
	public:
		Model(DrawBatch& batch, mat::Flag<mat::Tag::TRANSFORM> transform = mat::TRANSFORM_STD);
		MatInst& attach(MeshData& mesh, const Material& mat); //TODO: returning MatInst& directly is not safe!

		void setTransform();
		inline void updateTransforms() {this->setTransform();}
		void setVis(bool vis);
		bool isVis() const;
	public: //uncertain/todos/for debug:
		detail::MeshHandles& getMesh(i32 index);
		void upload(detail::MeshHandles& mesh);
		opengl::GPUShader& getShaderFor(detail::MeshHandles& mesh);
		auto begin() {
			return _meshes.begin();
		}
		auto end() {
			return _meshes.end();
		}
	public:
		Transforms transforms = {};
	protected:
		friend class DrawBatch;
		detail::MeshHandles& attachCustom(i32 vCount, i32 iCount, const Material& mat);
	protected:
		DrawBatch* _batch;
		mat::Flag<mat::Tag::TRANSFORM> _transformType = mat::TRANSFORM_STD;
		std::vector<detail::MeshHandles> _meshes = {};
		detail::ModelHandle _modelHandle;
		bool _isVis = true;
	};
	static_assert(std::is_nothrow_move_assignable_v<Model>);
	static_assert(!std::is_copy_assignable_v<Model>);
}