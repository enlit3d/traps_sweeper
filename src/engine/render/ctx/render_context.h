#pragma once
#include <memory>
#include "src/basic_types.h"
#include "impl/mnged_res.h"


namespace engine::render {
	class ShaderFac;

	class RenderContext : NotCopyOrMoveable {
	public:
		RenderContext();
		~RenderContext();

		template <typename T>
		Ref<T> acquire();

		ShaderFac& shaderFac();

	private:
		struct Impl;
		std::unique_ptr<Impl> _impl;
	};

}