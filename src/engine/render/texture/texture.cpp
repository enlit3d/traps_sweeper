#include "texture.h"
#include "image_data.h"
#include <cassert>

namespace engine::render {
	Texture::Texture()
		: _src{ std::make_shared<opengl::GPUTexture>() }
	{
	}
	Texture::Texture(Texture& src, Rect rect)
		: _src(src._src), _region(rect)
	{
	}
	Texture::UVRegion Texture::uvs() const
	{
		if (!_region) {
			return UVRegion{ 0.0f, 0.0f, 1.0f, 1.0f };
		}
		else {
			auto srcW = static_cast<f32>(_src->w());
			auto srcH = static_cast<f32>(_src->h());

			if (srcW == 0.0f || srcH == 0.0f){
				return UVRegion{ 0.0f, 0.0f, 1.0f, 1.0f };
			}

			auto x = (f32)_region.x;
			auto y = (f32)_region.y;
			auto w = (f32)_region.w;
			auto h = (f32)_region.h;

			return UVRegion{
				(x) / srcW,
				(y) / srcH,
				(x + w) / srcW,
				(y + h) / srcH,
			};
		}
	}
	void Texture::write(const ImageData& imageData)
	{
		if (_region) {
			assert(imageData.w == _region.w && imageData.h == _region.h);
			auto& r = _region;
			_src->write(imageData.data(), r.x, r.y, r.w, r.h);
		}
		else {
			assert(imageData.w == src().w() && imageData.h == src().h());
			_src->write(imageData.data());
		}
	
	}
	opengl::GPUTexture& Texture::src()
	{
		return *_src;
	}
	const opengl::GPUTexture& Texture::src() const {
		return *_src;
	}
	opengl::GPUTexture& Texture::operator*()
	{
		return this->src();
	}
	opengl::GPUTexture* Texture::operator->()
	{
		return &(this->src());
	}
	i32 Texture::w() const
	{
		if (_region) {
			return _region.w;
		}
		else {
			return _src->w();
		}
	}
	i32 Texture::h() const
	{
		if (_region) {
			return _region.h;
		}
		else {
			return _src->h();
		}
	}
}