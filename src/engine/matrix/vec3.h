#pragma once

#include "impl/matrix_base.h"

namespace engine::matrix
{
	class Vec3 : public MatrixBase<Vec3, f32, 3>{
	public:
		Vec3(f32 x, f32 y, f32 z);
		Vec3();
		Vec3& crossOf(const Vec3& a, const Vec3& b);
		Vec3& normalize();
		Vec3& setLength(f32 len);
		f32 lengthSqr();
		f32 length();
	};
	
}
