#include "gl_texture.h"
#include "defines/gl_type_enums.h"

#define GLEW_STATIC 
#include <GL/glew.h>
#include "error_handling.h"
#include <cassert>

//#include "src/engine/utils/console.h"

namespace engine::render::opengl
{
	struct GPUTexture::Impl {
		GLuint _texture = 0;
		TextureParams _params = {};
		i32 _w = 0;
		i32 _h = 0;
		bool _dirty = true;

		Impl() {
			glGenTextures(1, &_texture);
			//console::out(std::to_string(_texture));
			//checkError();
		}
		~Impl() {
			if (_texture != 0) {// spec ignores 0s so this isnt technically neccessary, but why the heck not
				glDeleteTextures(1, &_texture); 
				_texture = 0;
			}
		}
	};
	GPUTexture::GPUTexture()
		: _impl(std::make_unique<Impl>())
	{
	}
	GPUTexture::~GPUTexture() = default;
	GPUTexture::GPUTexture(GPUTexture &&) noexcept = default;
	GPUTexture & GPUTexture::operator=(GPUTexture &&) noexcept = default;

	GLenum toGLEnum(TextureParams::Format format) {
		switch (format) {
		case TextureParams::Format::RGBA:
			return GL_RGBA;
		default:
			throw;
		}
	}

	GLint toFilterEnum(TextureParams::Filter filter) {
		switch (filter) {
		case TextureParams::Filter::NEAREST:
			return GL_NEAREST;
		case TextureParams::Filter::LINEAR:
			return GL_LINEAR;
		case TextureParams::Filter::NEAREST_MIPMAP_NEAREST:
			return GL_NEAREST_MIPMAP_NEAREST;
		case TextureParams::Filter::LINEAR_MIPMAP_NEAREST:
			return GL_LINEAR_MIPMAP_NEAREST;
		case TextureParams::Filter::NEAREST_MIPMAP_LINEAR:
			return GL_NEAREST_MIPMAP_LINEAR;
		case TextureParams::Filter::LINEAR_MIPMAP_LINEAR:
			return GL_LINEAR_MIPMAP_LINEAR;
		default:
			throw;
		}
	}

	GLint toWrapEnum(TextureParams::Wrap warp) {
		switch (warp) {
		case TextureParams::Wrap::CLAMP_TO_EDGE:
			return GL_CLAMP_TO_EDGE;
		case TextureParams::Wrap::MIRRORED_REPEAT:
			return GL_MIRRORED_REPEAT;
		case TextureParams::Wrap::REPEAT:
			return GL_REPEAT;
		default:
			throw;
		}
	}

	GLint getInternalFormat(DataType type, TextureParams::Format format) {
		switch (format) {
		case TextureParams::Format::RGBA:
		{
			switch (type.toInt()) {
			case DataType::F32:
				return GL_RGBA32F;
			case DataType::U8:
				return GL_RGBA8;
			default:
				throw;
			}
		}
		break;
		default:
			throw;
		}
	}

	const TextureParams& GPUTexture::params() const
	{
		return _impl->_params;
	}

	TextureParams& GPUTexture::params(bool)
	{
		_impl->_dirty = true;
		return _impl->_params;
	}

	void GPUTexture::use()
	{
		checkError();
		glBindTexture(GL_TEXTURE_2D, _impl->_texture);
		if (_impl->_dirty) {
			auto& params = _impl->_params;
			if (params.magFilter == params.LINEAR_MIPMAP_LINEAR
				|| params.magFilter == params.LINEAR_MIPMAP_NEAREST
				|| params.magFilter == params.NEAREST_MIPMAP_LINEAR
				|| params.magFilter == params.NEAREST_MIPMAP_NEAREST
			) {
				params.magFilter = params.LINEAR;
				// magFilter can only be NEAREST OR LINEAR!
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toFilterEnum(params.minFilter));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toFilterEnum(params.magFilter));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toWrapEnum(params.wrapS));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toWrapEnum(params.wrapT));

			if (params.minFilter != params.LINEAR && params.minFilter != params.NEAREST) {
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			checkError();
			_impl->_dirty = false;
		}
	}

	void GPUTexture::resizeAndClear(i32 w, i32 h) // clears data!
	{
		checkError();
		//console::out("texture resize");
		_impl->_w = w;
		_impl->_h = h;
		auto& params = _impl->_params;
		this->use();
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			getInternalFormat(params.dataType, params.format),
			w,
			h,
			0,
			toGLEnum(params.format),
			toGLEnum(params.dataType),
			nullptr
		);
		checkError();
		if (params.minFilter != params.LINEAR && params.minFilter != params.NEAREST) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		checkError();
	}
	void GPUTexture::write(const byte* bytes)
	{
		write(bytes, 0, 0, _impl->_w, _impl->_h);
	}
	void GPUTexture::write(const byte* bytes, i32 x, i32 y, i32 w, i32 h)
	{
		assert(x >= 0 && y >= 0 && x + w <= _impl->_w && y + h <= _impl->_h);
		checkError();
		this->use();
		auto& params = _impl->_params;
		glTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			x,
			y,
			w,
			h,
			toGLEnum(params.format),
			toGLEnum(params.dataType),
			bytes
		);
		if (params.minFilter != params.LINEAR && params.minFilter != params.NEAREST) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		checkError();
	}
	i32 GPUTexture::byteSize()
	{
		auto& params = _impl->_params;
		auto pixels = _impl->_w * _impl->_h;
		auto channels = params.format == TextureParams::Format::RGBA ? 4 : 0;
		auto bytesPerChannel = params.dataType.bytesPerElem();
		return pixels * channels * bytesPerChannel;
	}
	i32 GPUTexture::w() const
	{
		return _impl->_w;
	}
	i32 GPUTexture::h() const
	{
		return _impl->_h;
	}
	bool GPUTexture::operator==(const GPUTexture& other) const
	{
		return this == &other;
	}
}