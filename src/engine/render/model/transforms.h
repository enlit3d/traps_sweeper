#pragma once

#include "src/engine/matrix/vec3.h"
#include "src/engine/matrix/quat.h"
#include "src/engine/matrix/mat4.h"

namespace engine::render {
	class Transforms {
	public:
		Transforms(const Transforms& other) = delete;
		matrix::Vec3 pos = {0.0f, 0.0f, 0.0f};
		matrix::Quat rot = {};
		matrix::Vec3 scl = {1.0f, 1.0f, 1.0f};
	};
}