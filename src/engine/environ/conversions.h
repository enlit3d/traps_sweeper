#pragma once

#include "src/engine/render/texture/image_data.h"

struct SDL_Surface;

namespace engine::env::conversions {	
	render::ImageData toImageDataAndDelete(SDL_Surface* surf);
}