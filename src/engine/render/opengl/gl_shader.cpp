#include "gl_shader.h"

#define GLEW_STATIC 
#include <GL/glew.h>

#include <iostream>
#include <unordered_map>
#include <utility>

#include "src/string_type.h"
#include "defines/gl_type_enums.h"
#include "error_handling.h"

#include "src/engine/render/chunk/attr_chunk.h"
#include "src/engine/render/chunk/impl/gpu_buffers.h"
#include "gl_texture.h"

namespace engine::render::opengl
{

	struct GPUShader::Impl {
		GLuint _program = 0;

		Impl() {};
		~Impl() {
			if (_program != 0) {
				glDeleteProgram(_program);
			}
		}

		struct VarInfo {
			str_t name = "";
			GLuint location = 0;
			GLenum type = 0;
		};

		using VarMap = std::unordered_map<str_t, VarInfo>;
		VarMap _attribs = {};
		VarMap _uniforms = {};

		GLuint createShader(const GLchar** source, GLuint shaderType) {
			auto shader = glCreateShader(shaderType);
			glShaderSource(shader, 1, source, NULL);
			glCompileShader(shader);
			auto shaderCompiled = GL_FALSE;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);
			if (shaderCompiled != GL_TRUE)
			{
				std::cout << "Unable to compile shader " << shader << "\n";
				
				i32 charLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &charLength);

				str_t str = "";
				str.reserve(charLength);
				glGetShaderInfoLog(shader, charLength, NULL, &str[0]);

				std::cout << str.c_str() << "\n";
				std::cout << *source;
				throw;
				return 0;
			}
			checkError();
			return shader;
		}
		GLuint makeProgram(const str_t& vertexSrc, const str_t& fragmentSrc) {
			auto vSrc = vertexSrc.c_str();
			auto fSrc = fragmentSrc.c_str();
			auto vertexShader = createShader(&vSrc, GL_VERTEX_SHADER);
			auto fragmentShader = createShader(&fSrc, GL_FRAGMENT_SHADER);
			if (!vertexShader || !fragmentShader) {
				return 0;
			}
			auto program = glCreateProgram();
			glAttachShader(program, vertexShader);
			glAttachShader(program, fragmentShader);

			glLinkProgram(program);

			// check for errors
			GLint programSuccess = GL_TRUE;
			glGetProgramiv(program, GL_LINK_STATUS, &programSuccess);
			if (programSuccess != GL_TRUE)
			{
				std::cout << "Error linking program " << program << "\n";

				int charLength = 0;

				//Get info string length
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &charLength);

				//Allocate string
				str_t str;
				str.reserve(charLength);

				//Get info log
				glGetProgramInfoLog(program, charLength, NULL, &str[0]);
				if (charLength > 0)
				{
					std::cout << str.c_str() << "\n";
				}
				throw;
				return 0;
			}
			else {
				return program;
			}
		}

		void updateVarInfos() {
			GLint count;

			GLint size; // size of the variable
			GLenum type; // type of the variable (float, vec3 or mat4, etc)

			const GLsizei bufSize = 256; // maximum name length
			GLchar name[bufSize]; // variable name in GLSL
			GLsizei length; // name length
			checkError();
			this->use();
			glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES, &count);
			for (GLuint i = 0; i < static_cast<GLuint>(count); ++i) {
				glGetActiveAttrib(_program, i, bufSize, &length, &size, &type, name);
				GLuint location = glGetAttribLocation(_program, name);
				glEnableVertexAttribArray(location);

				_attribs[name] = VarInfo{ name, location, type };
			}

			glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &count);
			for (GLuint i = 0; i < static_cast<GLuint>(count); ++i) {
				glGetActiveUniform(_program, i, bufSize, &length, &size, &type, name);
				GLuint location = static_cast<GLuint>(glGetUniformLocation(_program, name)); //glGetUniformLocation returns GLint for some reason... 
				_uniforms[name] = VarInfo{ name, location, type };
			}
		}

		void use() {
			static GLuint CURRENT_USED_PROGRAM = -1;
			if (CURRENT_USED_PROGRAM != _program) {
				CURRENT_USED_PROGRAM = _program;
				glUseProgram(_program);
			}
		}
	};


	GPUShader::GPUShader()
		: _impl(std::make_unique<Impl>())
	{
	}

	GPUShader::GPUShader(str_ref vertexSrc, str_ref fragSrc)
		: GPUShader()
	{
		_impl->_program = _impl->makeProgram(vertexSrc, fragSrc);
		checkError();
		_impl->updateVarInfos();
	}

	GPUShader::GPUShader(GPUShader &&) noexcept = default;

	GPUShader & GPUShader::operator=(GPUShader &&) noexcept = default;

	GPUShader::~GPUShader() = default;

	void GPUShader::use()
	{
		_impl->use();
		checkError();
	}

	bool GPUShader::hasUniform(str_ref name) const
	{
		return _impl->_uniforms.count(name) > 0;
	}

	void GPUShader::setUniform(str_ref name, const f32* data)
	{
		if (!hasUniform(name)) { return; }
		use();
		auto& info = _impl->_uniforms[name];
		switch (info.type) {
		case GL_FLOAT_VEC2:
			glUniform2fv(info.location, 1, data);
			break;
		case GL_FLOAT_VEC3:
			glUniform3fv(info.location, 1, data);
			break;
		case GL_FLOAT_VEC4:
			glUniform4fv(info.location, 1, data);
			break;
		case GL_FLOAT_MAT4:
			glUniformMatrix4fv(info.location, 1, GL_FALSE, data);
			break;
		default:
			throw; // not implemented
		}
		checkError();
	}

	void GPUShader::setUniform(str_ref name, GPUTexture & texture, i32 slot)
	{
		if (!hasUniform(name)) { return; }
		checkError();
		this->use();
		auto& info = _impl->_uniforms[name];

		switch (info.type) {
		case GL_SAMPLER_2D:
			glActiveTexture(GL_TEXTURE0 + slot);
			texture.use();
			glUniform1i(info.location, slot);
			glActiveTexture(GL_TEXTURE0);
			break;
		default:
			throw;
		}

		checkError();
	}

	bool GPUShader::hasAttrib(str_ref name) const
	{
		return _impl->_attribs.count(name) > 0;
	}

	void GPUShader::setAttribs(const AttrChunk & chunk)
	{
		// chunk must be uploaded before this is called (due to lazy initialization)
		auto& gpuBuffers = chunk.getGPUBuffers();
		use();
		for (auto& b : gpuBuffers) {
			auto& s = b._settings;
			auto& buf = b._buffer;
			auto& name = s.glAttribName;
			if (s.isIndices) {
				b._buffer.use(); // bind indices
				continue;
			}
			if (!this->hasAttrib(name)) { 
				continue;
			}
			auto loc = _impl->_attribs[name].location;
			b._buffer.use();
			glVertexAttribPointer(
				loc,
				b._settings.glComponentsPerVertex,
				toGLEnum(s.dataType),
				s.glNormalized ? GL_TRUE : GL_FALSE,
				s.glStride,
				reinterpret_cast<void*>(s.glStride)
			);
			checkError();
		}
	}
}