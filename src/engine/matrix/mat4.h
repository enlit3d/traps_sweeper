#pragma once
#include "impl/matrix_base.h"

namespace engine::matrix
{
	class Vec3;
	class Quat;
	
	class Mat4 : public MatrixBase<Mat4, f32, 16> {
	public:
		Mat4();
		Mat4(const Vec3& scl, const Quat& rot, const Vec3& pos);
		Mat4& identity();

		Mat4& productOf(const Mat4& a, const Mat4& b);
		Mat4& invert();
		Mat4& transpose();

		Mat4& orthogonal(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);
		Mat4& perspective(f32 fovRadian, f32 aspect, f32 near, f32 far);
		Mat4& lookDir(const Vec3& pos, const Vec3& dir);
		Mat4& lookDir(const Vec3& pos, const Vec3& dir, const Vec3& up);
		
		Mat4& fromSRT(const Vec3& scl, const Quat& rot, const Vec3& pos);

		template <i32 Row>
		constexpr f32* getRow() {
			static_assert(Row >= 0 && Row < 4);
			return data() + Row * 4;
		}
		template <i32 Row, size_t N>
		constexpr void setRow(const f32(&data)[N]) {
			//changed from taking initializer_list to this form; see
			//https://stackoverflow.com/questions/5438671/static-assert-on-initializer-listsize
			static_assert(N == 4);
			auto row = getRow<Row>();
			row[0] = data[0];
			row[1] = data[1];
			row[2] = data[2];
			row[3] = data[3];
		}

	};
}