#include "error_handling.h"

#define GLEW_STATIC 
#include <GL/glew.h>

namespace engine::render::opengl
{
	void checkError() {
#ifdef NDEBUG
#else 
	auto err = glGetError();
	if (err) {
		throw;
	}
#endif

	}
}