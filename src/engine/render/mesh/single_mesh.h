#pragma once

#include "src/engine/collections/memory/raw_buffer.h"
#include "src/basic_types.h"

namespace engine::render
{
	using namespace engine::memory;
	class MeshData {
	public:
		MeshData(i32 vertices, i32 indices);
		ArrayView<f32> positions();
		ArrayView<f32> normals();
		ArrayView<f32> uvs();
		ArrayView<u8> colors();
		ArrayView<u16> indices();

		i32 vCount() const;
		i32 iCount() const;
	private:
		RawMemory _data;
		i32 _vertexCount;
		i32 _indexCount;
	};
}