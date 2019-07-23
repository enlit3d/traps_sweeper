#pragma once

#include "single_mesh.h"

namespace engine::render::mesh_gen {
	MeshData Plane(f32 w = 1.0f, f32 h = 1.0f) {
		auto mesh = engine::render::MeshData(4, 6);
		auto hW = w / 2.0f;
		auto hH = h / 2.0f;
		mesh.positions() = {
			hW, hH, 0.0f,
			-hW, hH, 0.0f,
			-hW, -hH, 0.0f,
			hW, -hH, 0.0f
		};
		mesh.uvs() = {
			1.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f
		};
		mesh.indices() = {
			0, 1, 2,
			0, 2, 3
		};

		return mesh;
	}
}