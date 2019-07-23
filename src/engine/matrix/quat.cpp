#include "quat.h"
#include "vec3.h"
#include "src/engine/utils/math.h"

namespace engine::matrix
{
	Quat::Quat()
	{
		identity();
	}
	Quat & Quat::identity()
	{
		setTo({0.0f, 0.0f, 0.0f, 1.0f});
		return *this;
	}
	Quat & Quat::axisAngle(const Vec3 & axis, f32 radians)
	{
		radians *= 0.5f;
		auto sinT = math::sin(radians);
		setTo({
			sinT * axis[0],
			sinT * axis[1],
			sinT * axis[2],
			math::cos(radians)
		});
		return *this;
	}
	Quat & Quat::productOf(const Quat & a, const Quat & b)
	{
		//r.v = v*q.w + q.v*w + v.Cross(q.v);
		//r.w = w*q.w - v.Dot(q.v);

		auto ax = a[0], ay = a[1], az = a[2], aw = a[3],
			bx = b[0], by = b[1], bz = b[2], bw = b[3];
		setTo({
			ax * bw + bx * aw + ay * bz - az * by,
			ay * bw + by * aw + az * bx - ax * bz,
			az * bw + bz * aw + ax * by - ay * bx,
			aw * bw - ax * bx - ay * by - az * bz
		});
		return *this;
	}
}