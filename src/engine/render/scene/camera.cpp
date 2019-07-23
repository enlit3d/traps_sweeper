#include "camera.h"

namespace engine::render {
	const matrix::Mat4 & BasicCamera::projection() const
	{
		return _projection;
	}
	matrix::Mat4 & BasicCamera::projection(bool)
	{
		_dirty = true;
		return _projection;
	}

	BasicCamera::Vectors BasicCamera::vectors(bool)
	{
		_dirty = true;
		auto v = Vectors{ _pos, _side, _up, _dir };
		if (&v.pos != &_pos) { throw; }
		return v;
	}
	BasicCamera::ConstVectors BasicCamera::vectors()
	{
		return ConstVectors{ _pos, _side, _up, _dir };
	}
	const matrix::Mat4 & BasicCamera::viewProj()
	{
		recalcMatrices();
		return _viewProj;
	}
	void BasicCamera::recalcMatrices()
	{
		if (!_dirty) { return; }
		_view.lookDir(_pos, _dir, _up);
		_viewProj.productOf(_projection, _view);
		_dirty = false;
	}
}