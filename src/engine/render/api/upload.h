#pragma once

#include "src/basic_types.h"

namespace engine::render {
	class AttrChunk;
	class DrawBatch;

	void upload(AttrChunk& chunk);
	void upload(DrawBatch& batch);
}