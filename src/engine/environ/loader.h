#pragma once

#include "src/string_type.h"
#include "../render/texture/texture.h"

namespace engine::env {
	
	class Loader {
	public:
		Loader();
		render::Texture loadImg(str_ref fileName);
	};
}