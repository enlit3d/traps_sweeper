#include "upload.h"

#include "src/engine/render/chunk/attr_chunk.h"
#include "src/engine/render/model/draw_batch.h"

namespace engine::render {
	void upload(AttrChunk& chunk)
	{
		chunk.upload();
	}
	void upload(DrawBatch& batch)
	{
		batch.upload();
	}
}