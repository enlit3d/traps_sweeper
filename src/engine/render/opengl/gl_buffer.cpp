#include "gl_buffer.h"

#define GLEW_STATIC 
#include <GL/glew.h>
#include <utility>
#include "error_handling.h"

//#include "src/engine/utils/console.h";


namespace engine::render::opengl
{
	struct GPUBuffer::Impl {
		i32 _size = 0;
		GLuint _bufferId = 0;
		GLuint _type = 0;

		Impl() {};
		~Impl() {
			glDeleteBuffers(1, &_bufferId);
		}
	};

	GPUBuffer::GPUBuffer(i64 size, EnumT type)
		: _impl(std::make_unique<Impl>())
	{
		_impl->_type = type;
		resize(size);
	}

	GPUBuffer::~GPUBuffer() = default;
	GPUBuffer::GPUBuffer(GPUBuffer &&) noexcept = default;
	GPUBuffer& GPUBuffer::operator=(GPUBuffer &&) noexcept = default;

	void GPUBuffer::use() const
	{
		glBindBuffer(_impl->_type, _impl->_bufferId);
	}

	void GPUBuffer::resize(i64 newSize) {
		if (newSize <= _impl->_size) { return; }
		if (_impl->_size == 0) {
			glGenBuffers(1, &(_impl->_bufferId));
			glBindBuffer(_impl->_type, _impl->_bufferId);
			glBufferData(_impl->_type, newSize, nullptr, GL_STATIC_DRAW);
			checkError();
		}
		else {
			GLuint newBuffer = 0;
			glGenBuffers(1, &newBuffer);
			glBindBuffer(GL_COPY_WRITE_BUFFER, newBuffer);
			glBufferData(GL_COPY_WRITE_BUFFER, newSize, nullptr, GL_STATIC_DRAW);
			glBindBuffer(GL_COPY_READ_BUFFER, _impl->_bufferId);
			glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, _impl->_size);
			glDeleteBuffers(1, &(_impl->_bufferId));
			checkError();
			_impl->_bufferId = newBuffer;
			//console::out("glBuffer resized");
		}
		_impl->_size = newSize;
	}

	void GPUBuffer::write(void * data, i64 offset, i64 bytes) {
		glBindBuffer(_impl->_type, _impl->_bufferId);
		glBufferSubData(_impl->_type, offset, bytes, data);
		checkError();
	}

	void GPUBuffer::read(void * output, i64 offset, i64 bytes) {
		glBindBuffer(_impl->_type, _impl->_bufferId);
		glGetBufferSubData(_impl->_type, offset, bytes, output);
		checkError();
	}

	GPUBuffer::EnumT GPUBuffer::VERTEX_BUFFER = GL_ARRAY_BUFFER;
	GPUBuffer::EnumT GPUBuffer::INDEX_BUFFER = GL_ELEMENT_ARRAY_BUFFER;
}