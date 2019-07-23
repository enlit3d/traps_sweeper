#pragma once

#include "src/basic_types.h"

namespace engine::render
{
	class AttrChunk;
	class DrawBatch;
	class BasicCamera;
	class Model;

	namespace opengl {
		class GPUShader;
	}

	i32 drawWithoutUpload(const AttrChunk& chunk, opengl::GPUShader& shader);
	i32 drawWithoutUpload(DrawBatch& batch, BasicCamera& camera);

	i32 draw(Model& model, BasicCamera& camera);
}