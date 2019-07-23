#include "loader.h"
#include <SDL_image.h>

#include <cassert>
#include <iostream>

namespace engine::env
{
	Loader::Loader()
	{
		auto flags = IMG_INIT_JPG | IMG_INIT_PNG;
		auto result = IMG_Init(flags);
		assert((result & flags));
	}

	render::Texture Loader::loadImg(str_ref fileName)
	{  
		auto image = IMG_Load(fileName.c_str());
		if (image) {
			auto bytesPerPixel = image->format->BytesPerPixel;
			auto targetFormat = SDL_PIXELFORMAT_ABGR8888; // always target 32 bit textures as 24 bit textures may have to be emulated in driver and is slower
			if (image->format->format != targetFormat) {
				std::cout << "LOADER: Converting image format for \"" << fileName << "\"\n";
				auto convertedSurf = SDL_ConvertSurfaceFormat(image, targetFormat, 0);
				if (convertedSurf) {
					SDL_FreeSurface(image);
					image = convertedSurf;
				}
				else {
					std::cout << "WARNING: Failed to convert image to appropriate format. \n";
				}
			}
			auto tex = render::Texture{};
			auto& params = tex->params(true);
			params.format = render::opengl::TextureParams::Format::RGBA;
			tex->resizeAndClear(image->w, image->h);
			tex->write(static_cast<byte*>(image->pixels));
			SDL_FreeSurface(image);
			return tex;
		}
		else {
			std::cout << "WARNING: Loading Texture Failed. " << IMG_GetError() << "\n";
		}
		return render::Texture{};
	}


}