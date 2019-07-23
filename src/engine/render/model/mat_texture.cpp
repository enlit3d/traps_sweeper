#include "mat_texture.h"

namespace engine::render {
	namespace detail {

		void writeSRTTo(f32* dest, const matrix::Vec3& scl, const matrix::Quat& rot, const matrix::Vec3& pos)
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

			dest[0] = (1.0f - (yy + zz)) * scl[0];
			dest[1] = (xy + wz) * scl[0];
			dest[2] = (xz - wy) * scl[0];
			dest[3] = 0.0f;

			dest[4] = (xy - wz) * scl[1];
			dest[5] = (1.0f - (xx + zz)) * scl[1];
			dest[6] = (yz + wx) * scl[1];
			dest[7] = 0.0f;

			dest[8] = (xz + wy) * scl[2];
			dest[9] = (yz - wx) * scl[2];
			dest[10] = (1.0f - (xx + yy)) * scl[2];
			dest[11] = 0.0f;

			dest[12] = pos[0];
			dest[13] = pos[1];
			dest[14] = pos[2];
			dest[15] = 1.0f;
		}


		void TransformsProxy::write(memory::ArrayView<f32>& buffer)
		{
			writeSRTTo(buffer.data(), this->scl, this->rot, this->pos);
		}
	}
}