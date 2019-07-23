#include "matrix_base.h"
#include "../vec3.h"
#include "../mat4.h"
#include "../quat.h"
#include <algorithm>

namespace engine::matrix
{

	// explicit instantiations: aka "allowed" types
	template class MatrixBase<Vec3, f32, 3>;
	template class MatrixBase<Quat, f32, 4>;
	template class MatrixBase<Mat4, f32, 16>;
}