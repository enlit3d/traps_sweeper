#include "mat4.h"
#include "vec3.h"
#include "quat.h"

namespace engine::matrix
{
	//TODO: SIMD 

	Mat4::Mat4()
	{
		identity();
	}
	Mat4::Mat4(const Vec3& scl, const Quat& rot, const Vec3& pos)
	{
		this->fromSRT(scl, rot, pos);
	}
	Mat4 & Mat4::identity()
	{
		setRow<0>({ 1.0f, 0.0f, 0.0f, 0.0f });
		setRow<1>({ 0.0f, 1.0f, 0.0f, 0.0f });
		setRow<2>({ 0.0f, 0.0f, 1.0f, 0.0f });
		setRow<3>({ 0.0f, 0.0f, 0.0f, 1.0f });
		return *this;
	}
	Mat4 & Mat4::productOf(const Mat4 & a, const Mat4 & b)
	{
		auto a00 = a[0], a01 = a[1], a02 = a[2], a03 = a[3],
			a10 = a[4], a11 = a[5], a12 = a[6], a13 = a[7],

			a20 = a[8], a21 = a[9], a22 = a[10], a23 = a[11],
			a30 = a[12], a31 = a[13], a32 = a[14], a33 = a[15];

		auto b0 = b[0], b1 = b[1], b2 = b[2], b3 = b[3];


		setRow<0>({
			b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30,
			b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31,
			b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32,
			b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33
		});

		b0 = b[4]; b1 = b[5]; b2 = b[6]; b3 = b[7];
		setRow<1>({
			b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30,
			b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31,
			b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32,
			b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33
		});

		b0 = b[8]; b1 = b[9]; b2 = b[10]; b3 = b[11];
		setRow<2>({
			b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30,
			b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31,
			b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32,
			b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33
		});

		b0 = b[12]; b1 = b[13]; b2 = b[14]; b3 = b[15];
		setRow<3>({
			b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30,
			b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31,
			b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32,
			b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33
		});

		return *this;
	}
	Mat4 & Mat4::invert()
	{
		Mat4& self = *this;
		auto a00 = self[0], a01 = self[1], a02 = self[2], a03 = self[3],
			a10 = self[4], a11 = self[5], a12 = self[6], a13 = self[7],
			a20 = self[8], a21 = self[9], a22 = self[10], a23 = self[11],
			a30 = self[12], a31 = self[13], a32 = self[14], a33 = self[15];

		auto
			b00 = a00 * a11 - a01 * a10,
			b01 = a00 * a12 - a02 * a10,
			b02 = a00 * a13 - a03 * a10,
			b03 = a01 * a12 - a02 * a11,
			b04 = a01 * a13 - a03 * a11,
			b05 = a02 * a13 - a03 * a12,
			b06 = a20 * a31 - a21 * a30,
			b07 = a20 * a32 - a22 * a30,
			b08 = a20 * a33 - a23 * a30,
			b09 = a21 * a32 - a22 * a31,
			b10 = a21 * a33 - a23 * a31,
			b11 = a22 * a33 - a23 * a32,

			// Calculate the determinant
			det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

		if (!det) {
			return *this;
		}
		det = 1.0f / det;

		self[0] = (a11 * b11 - a12 * b10 + a13 * b09) * det;
		self[1] = (a02 * b10 - a01 * b11 - a03 * b09) * det;
		self[2] = (a31 * b05 - a32 * b04 + a33 * b03) * det;
		self[3] = (a22 * b04 - a21 * b05 - a23 * b03) * det;
		self[4] = (a12 * b08 - a10 * b11 - a13 * b07) * det;
		self[5] = (a00 * b11 - a02 * b08 + a03 * b07) * det;
		self[6] = (a32 * b02 - a30 * b05 - a33 * b01) * det;
		self[7] = (a20 * b05 - a22 * b02 + a23 * b01) * det;
		self[8] = (a10 * b10 - a11 * b08 + a13 * b06) * det;
		self[9] = (a01 * b08 - a00 * b10 - a03 * b06) * det;
		self[10] = (a30 * b04 - a31 * b02 + a33 * b00) * det;
		self[11] = (a21 * b02 - a20 * b04 - a23 * b00) * det;
		self[12] = (a11 * b07 - a10 * b09 - a12 * b06) * det;
		self[13] = (a00 * b09 - a01 * b07 + a02 * b06) * det;
		self[14] = (a31 * b01 - a30 * b03 - a32 * b00) * det;
		self[15] = (a20 * b03 - a21 * b01 + a22 * b00) * det;

		return self;
	}
	Mat4 & Mat4::transpose()
	{
		Mat4& self = *this;
		auto m1 = self[1],
			m2 = self[2],
			m3 = self[3],
			m6 = self[6],
			m7 = self[7];

		self[1] = self[4];
		self[2] = self[8];
		self[3] = self[12];
		self[4] = m1;

		self[6] = self[9];
		self[7] = self[13];
		self[8] = m2;
		self[9] = m6;

		self[11] = self[14];
		self[12] = m3;
		self[13] = m7;
		self[14] = self[11];
		return self;
	}
	Mat4 & Mat4::orthogonal(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far)
	{
		auto lr = 1.0f / (left - right),
			bt = 1.0f / (bottom - top),
			nf = 1.0f / (near - far);

		setRow<0>({ -2.0f * lr, 0.0f, 0.0f, 0.0f });
		setRow<1>({ 0.0f, -2.0f * bt, 0.0f, 0.0f });
		setRow<2>({ 0.0f, 0.0f, 2.0f * nf, 0.0f } );
		setRow<3>({ (left + right) * lr, (top + bottom) * bt, (far + near) * nf, 1.0f });

		return *this;
	}
	Mat4 & Mat4::perspective(f32 fovRadian, f32 aspect, f32 near, f32 far)
	{
		auto f = 1.0f / std::tan(fovRadian / 2.0f),
			nf = 1.0f / (near - far);

		setRow<0>({ f / aspect, 0.0f, 0.0f, 0.0f });
		setRow<1>({ 0.0f, f, 0.0f, 0.0f });
		setRow<2>({ 0.0f, 0.0f, (far + near)*nf, -1.0f });
		setRow<3>({ 0.0f, 0.0f, (2.0f * far * near) * nf, 0.0f });

		return *this;
	}
	Mat4 & Mat4::lookDir(const Vec3 & pos, const Vec3 & dir)
	{
		return lookDir(pos, dir, Vec3{ 0.0f, 1.0f, 0.0f });
	}
	Mat4 & Mat4::lookDir(const Vec3 & pos, const Vec3 & dir, const Vec3 & up)
	{
		f32 x0, x1, x2, y0, y1, y2, z0, z1, z2, len,
			eyex = pos[0],
			eyey = pos[1],
			eyez = pos[2],
			upx = up[0],
			upy = up[1],
			upz = up[2];

		z0 = -dir[0];
		z1 = -dir[1];
		z2 = -dir[2];
		len = 1.0f / std::sqrt(z0 * z0 + z1 * z1 + z2 * z2);
		z0 *= len;
		z1 *= len;
		z2 *= len;
		x0 = upy * z2 - upz * z1;
		x1 = upz * z0 - upx * z2;
		x2 = upx * z1 - upy * z0;
		len = std::sqrt(x0 * x0 + x1 * x1 + x2 * x2);
		if (!len) {
			x0 = 0.0f;
			x1 = 0.0f;
			x2 = 0.0f;
		}
		else {
			len = 1.0f / len;
			x0 *= len;
			x1 *= len;
			x2 *= len;
		}
		y0 = z1 * x2 - z2 * x1;
		y1 = z2 * x0 - z0 * x2;
		y2 = z0 * x1 - z1 * x0;
		len = std::sqrt(y0 * y0 + y1 * y1 + y2 * y2);
		if (!len) {
			y0 = 0.0f;
			y1 = 0.0f;
			y2 = 0.0f;
		}
		else {
			len = 1.0f / len;
			y0 *= len;
			y1 *= len;
			y2 *= len;
		}

		Mat4& self = *this;
		self[0] = x0;
		self[1] = y0;
		self[2] = z0;
		self[3] = 0;
		self[4] = x1;
		self[5] = y1;
		self[6] = z1;
		self[7] = 0;
		self[8] = x2;
		self[9] = y2;
		self[10] = z2;
		self[11] = 0;
		self[12] = -(x0 * eyex + x1 * eyey + x2 * eyez);
		self[13] = -(y0 * eyex + y1 * eyey + y2 * eyez);
		self[14] = -(z0 * eyex + z1 * eyey + z2 * eyez);
		self[15] = 1;
		return self;
	}
	Mat4 & Mat4::fromSRT(const Vec3 & scl, const Quat & rot, const Vec3 & pos)
	{
		auto x = rot[0];
		auto y = rot[1];
		auto z = rot[2];
		auto w = rot[3];

		auto x2 = x + x;
		auto y2 = y + y;
		auto z2 = z + z;
		auto xx = x * x2;
		auto xy = x * y2;
		auto xz = x * z2;
		auto yy = y * y2;
		auto yz = y * z2;
		auto zz = z * z2;
		auto wx = w * x2;
		auto wy = w * y2;
		auto wz = w * z2;

		auto& self = *this;

		self[0] = (1.0f - (yy + zz)) * scl[0];
		self[1] = (xy + wz) * scl[0];
		self[2] = (xz - wy) * scl[0];
		self[3] = 0.0f;

		self[4] = (xy - wz) * scl[1];
		self[5] = (1.0f - (xx + zz)) * scl[1];
		self[6] = (yz + wx) * scl[1];
		self[7] = 0.0f;

		self[8] = (xz + wy) * scl[2];
		self[9] = (yz - wx) * scl[2];
		self[10] = (1.0f - (xx + yy)) * scl[2];
		self[11] = 0.0f;

		self[12] = pos[0];
		self[13] = pos[1];
		self[14] = pos[2];
		self[15] = 1.0f;

		return *this;
	}
}