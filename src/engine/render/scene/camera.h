#pragma once
#include "src/engine/matrix/vec3.h"
#include "src/engine/matrix/mat4.h"

namespace engine::render
{
	class BasicCamera {
	public:
		struct Vectors {
			using Ref = matrix::Vec3&;
			Ref pos;
			Ref side;
			Ref up;
			Ref dir;
		};
		struct ConstVectors {
			using Ref = const matrix::Vec3&;
			Ref pos;
			Ref side;
			Ref up;
			Ref dir;
		};

		const matrix::Mat4& projection() const;
		matrix::Mat4& projection(bool);

		Vectors vectors(bool);
		ConstVectors vectors();

		const matrix::Mat4& viewProj();

	private:
		matrix::Vec3 _pos = { 0.0f, 0.0f, 1.0f };
		matrix::Vec3 _side = { 1.0f, 0.0f, 0.0f };
		matrix::Vec3 _up = { 0.0f, 1.0f, 0.0f };
		matrix::Vec3 _dir = { 0.0f, 0.0f, -1.0f };

		matrix::Mat4 _projection = {};
		matrix::Mat4 _view = {};
		matrix::Mat4 _viewProj = {};

		bool _dirty = true;
	private:
		void recalcMatrices();
	};
}