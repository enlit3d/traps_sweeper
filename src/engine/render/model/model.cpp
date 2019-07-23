#include "model.h"
#include "draw_batch.h"
#include "../mesh/single_mesh.h"
#include "../material/material.h"
#include "../material/shader_fac.h"
#include "../ctx/render_context.h"
#include "src/engine/utils/math.h"

namespace engine::render
{
	Model::Model(DrawBatch & batch, mat::Flag<mat::Tag::TRANSFORM> transform)
		: _batch(&batch), _modelHandle(batch.getModelHandle(transform))
	{
		_transformType = transform;
	}
	
	/*
	Transforms& Model::transforms(bool)
	{
		return _transformHandle.get(true);
	}
	const Transforms& Model::transforms() const
	{
		return _transformHandle.get();
	}
	*/
	detail::MeshHandles& Model::attachCustom(i32 vCount, i32 iCount, const Material& matSrc)
	{
		auto mat = matSrc; // create copy
		mat.transform = this->_transformType;

		auto m = _batch->create(mat, vCount, iCount);

		auto& ptr = m.dataPtr;

		if (ptr.hasBuffer("a_mIndex")) {
			auto mIndices = ptr.getTempWriteBuffer<f32>("a_mIndex");

			f32 index = 0.0f;
			std::visit(
				[&index](auto&& elem) {
					index = static_cast<f32>(elem.index());
				}, 
				_modelHandle
			);

			for (auto& v : mIndices) {
				v = index;
			}
		}

		if (ptr.hasBuffer("a_matSlot")) {
			auto buffer = ptr.getTempWriteBuffer<f32>("a_matSlot");
			auto slot = static_cast<f32>(m.matInst._modelData.index());
			for (auto& v : buffer) {
				v = slot;
			}
		}

		_meshes.push_back(std::move(m));
		return _meshes.back();
	}

	MatInst& Model::attach(MeshData & mesh, const Material& matSrc)
	{
		auto& meshInst = this->attachCustom(mesh.vCount(), mesh.iCount(), matSrc);

		auto& ptr = meshInst.dataPtr;
		ptr.writeData("a_pos", mesh.positions());
		ptr.writeIndices(mesh.indices());

		if (ptr.hasBuffer("a_uv")) {
			for (auto& tInput : matSrc._textures) {
				auto uvs = tInput.texture.uvs();
				auto buffer = ptr.getTempWriteBuffer<f32>("a_uv");
				auto srcUVs = mesh.uvs();
				for (auto i = 0; i < buffer.count(); i += 2) {
					buffer[i] = math::lerp(uvs.u0, uvs.u1, srcUVs[i]);
					buffer[i+1] = math::lerp(uvs.v0, uvs.v1, srcUVs[i+1]);
				}
			}
			//ptr.writeData("a_uv", mesh.uvs());
		}

		return meshInst.matInst;
	}

	void Model::setTransform()
	{
		using namespace detail;
		auto index = _modelHandle.index();
		switch (index) {
			case ext::variant_index<detail::ModelHandle, TransformsHandle>() :
				std::get<TransformsHandle>(_modelHandle).view(true) = matrix::Mat4{ transforms.scl, transforms.rot, transforms.pos }.data();
			break;
			case ext::variant_index<detail::ModelHandle, PosHandle>() :
			{
				auto v = std::get<PosHandle>(_modelHandle).view(true);
				v[0] = transforms.pos[0];
				v[1] = transforms.pos[1];
				v[2] = transforms.pos[2];
				v[3] = 1.0f;
				
				//std::get<PosHandle>(_modelHandle).view(true) = { transforms.pos[0], transforms.pos[1], transforms.pos[2], 1.0f };
			}
			
			
			break;
			default:
				throw;
		}
	}
	void Model::setVis(bool vis)
	{
		if (_isVis == vis) { return; }
		for (auto& m : _meshes) {
			m.dataPtr.toggleDrawRange(vis);
		}
		_isVis = vis;
	}
	bool Model::isVis() const
	{
		return _isVis;
	}
	detail::MeshHandles& Model::getMesh(i32 index)
	{
		return _meshes.at(index);
	}
	void Model::upload(detail::MeshHandles& mesh) {
		mesh.dataPtr->upload();
	}
	opengl::GPUShader& Model::getShaderFor(detail::MeshHandles& mesh) {
		auto& shader = _batch->getShaderAndApplyBatchSettings(mesh.matInst);
		_batch->applyMatInstSettings(mesh.matInst, shader);
		return shader;
	}
}