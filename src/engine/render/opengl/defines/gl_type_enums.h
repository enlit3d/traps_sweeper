#pragma once

#include "gl_types.h"
#define GLEW_STATIC 
#include <GL\glew.h>

namespace engine::render::opengl 
{
	GLuint toGLEnum(DataType t);
}

