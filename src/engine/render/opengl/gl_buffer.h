#pragma once

#include "src/basic_types.h"
#include <memory>

namespace engine::render::opengl
{
	class GPUBuffer : NotCopyable {
	public:
		using EnumT = u32;
		GPUBuffer(i64 size, EnumT type=VERTEX_BUFFER);
		~GPUBuffer();
		GPUBuffer(GPUBuffer&&) noexcept;
		GPUBuffer& operator=(GPUBuffer&&) noexcept;
		void use() const;
		void resize(i64 newSize);
		void write(void* data, i64 offset, i64 bytes);
		void read(void* output, i64 offset, i64 bytes);

		static EnumT VERTEX_BUFFER;
		static EnumT INDEX_BUFFER;
	private:
		struct Impl;
		std::unique_ptr<Impl> _impl;
	};
	static_assert(std::is_nothrow_move_constructible_v<GPUBuffer>);
}