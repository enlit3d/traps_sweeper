#include "draw.h"

#define GLEW_STATIC 
#include <GL/glew.h>

#include "src/engine/render/opengl/gl_shader.h"
#include "src/engine/render/opengl/gl_texture.h"
#include "src/engine/render/opengl/error_handling.h"
#include "src/engine/render/chunk/attr_chunk.h"
#include "src/engine/render/model/draw_batch.h"
#include "src/engine/render/scene/camera.h"

namespace engine::render {
	i32 draw(Model& model, BasicCamera& camera)
	{
		auto drawn = 0;
		for (auto& mesh : model) {
			model.upload(mesh);
			auto& shader = model.getShaderFor(mesh);
			shader.setUniform("u_view", camera.viewProj().data());
			shader.use();
			shader.setAttribs(*mesh.dataPtr);
			opengl::checkError();
			auto count = mesh.dataPtr.iCount();
			auto start = mesh.dataPtr.iOffset();
			glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, (void*)(start * sizeof(u16)));
			drawn += (count)/3;
			opengl::checkError();
		}
		return drawn;
	}

	i32 drawWithoutUpload(const AttrChunk& chunk, opengl::GPUShader & shader)
	{
		//chunk.upload();
		shader.use();
		shader.setAttribs(chunk);
		opengl::checkError();
		auto& ranges = chunk.drawRanges();
		auto drawn = 0;
		for (auto r : ranges) {
			//TODO: u32 indices?
			glDrawElements(GL_TRIANGLES, r.end - r.start, GL_UNSIGNED_SHORT, (void *)(r.start * sizeof(u16)));
			drawn += r.end - r.start;
			opengl::checkError();
		}
		return drawn / 3;
	}
	i32 drawWithoutUpload(DrawBatch& batch, BasicCamera& camera)
	{
		return batch.drawAll(camera);
	}
}