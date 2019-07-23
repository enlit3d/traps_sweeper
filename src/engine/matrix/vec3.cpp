#include "vec3.h"
#include "src/engine/utils/math.h"

namespace engine::matrix
{
	Vec3::Vec3(f32 x, f32 y, f32 z)
		: MatrixBase(x, y, z)
	{}
	Vec3::Vec3()
		: MatrixBase(0.0f, 0.0f, 0.0f)
	{}
	Vec3 & Vec3::crossOf(const Vec3 & a, const Vec3 & b)
	{
		auto& self = *this;
		auto ax = a[0], ay = a[1], az = a[2],
			bx = b[0], by = b[1], bz = b[2];
		self[0] = ay * bz - az * by;
		self[1] = az * bx - ax * bz;
		self[2] = ax * by - ay * bx;

		return self;
	}
	Vec3& Vec3::normalize()
	{
		auto& self = *this;
		if (this->lengthSqr() == 0.0f) { return self; }
		auto r = 1.0f/math::sqrt(this->lengthSqr());
		self[0] *= r;
		self[1] *= r;
		self[2] *= r;
		return self;
	}
	Vec3& Vec3::setLength(f32 len)
	{
		auto lenSqr = this->lengthSqr();
		if (lenSqr == 0.0f) {
			this->identity();
			return *this;
		}
		else {
			this->normalize();
			this->operator*=(len);
			return *this;
		}
	}
	f32 Vec3::lengthSqr()
	{
		auto& self = *this;
		return self[0] * self[0] + self[1] * self[1] + self[2] * self[2];
	}
	f32 Vec3::length()
	{
		return math::sqrt(this->lengthSqr());
	}

	//constexpr auto SIZE = sizeof(Vec3);
}