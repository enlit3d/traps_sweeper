#include "render_context.h"
#include "../chunk/attr_chunk.h"
#include "../material/shader_fac.h"
#include "../model/mat_texture.h"

namespace engine::render {
	struct RenderContext::Impl {
		ShaderFac shaderFac = {};
		ResPool<AttrChunk> chunks = {};
		ResPool<detail::MatTexture> matTextures = {};
	};

	RenderContext::RenderContext()
		: _impl(std::make_unique<Impl>())
	{
	}

	RenderContext::~RenderContext() = default;

	ShaderFac& RenderContext::shaderFac()
	{
		return _impl->shaderFac;
	}

	/*
		Managed Resources
	*/

	/*
	template<>
	Ref<AttrChunk> RenderContext::acquire<AttrChunk>()
	{
		return _impl->chunks.acquire();
	}

	template<>
	Ref<detail::MatTexture> RenderContext::acquire<detail::MatTexture>()
	{
		return _impl->matTextures.acquire();
	}
	*/
}