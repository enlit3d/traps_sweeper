#include "gl_type_enums.h"



namespace engine::render::opengl
{
	GLuint toGLEnum(DataType t) {
		switch (t.value()) {
		case DataType::F32:
			return GL_FLOAT;
		case DataType::F64:
			return GL_DOUBLE;
		case DataType::U8:
			return GL_UNSIGNED_BYTE;
		case DataType::U16:
			return GL_UNSIGNED_SHORT;
		case DataType::U32:
			return GL_UNSIGNED_INT;
		case DataType::I8:
			return GL_BYTE;
		case DataType::I16:
			return GL_SHORT;
		case DataType::I32:
			return GL_INT;
		default:
			throw;
		}
	}
}