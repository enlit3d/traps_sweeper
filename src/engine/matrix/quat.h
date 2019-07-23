#pragma once 
#include "impl/matrix_base.h"

namespace engine::matrix
{
	class Vec3;

	class Quat : public MatrixBase<Quat, f32, 4> {
	public:
		Quat();
		Quat& identity();
		Quat& axisAngle(const Vec3& axis, f32 radians);
		Quat& productOf(const Quat& a, const Quat& b);
	};
}