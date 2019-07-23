#pragma once

#include "src/engine/render/chunk/layout.h"
#include "src/engine/render/opengl/gl_buffer.h"
#include <vector>

namespace engine::render
{
	class GPUBuffers {
	public:
		GPUBuffers() {};

		struct GPUData {
		public:
			i32 bytesPerElem() const {
				return _settings.bytesPerVertex();
			}
		public:
			Layout::AttrSetting _settings;
			opengl::GPUBuffer _buffer;
		};
		static_assert(!std::is_copy_assignable_v<GPUData>);

		void init(Layout layout) { //TODO: this can be done more efficiently than clear and repopulate
			_buffers.clear();
			for (auto& s : layout) {
				emplace(s);
			}
		}

		void emplace(Layout::AttrSetting settings) {
			auto buf = GPUData{ settings, opengl::GPUBuffer{0, settings.isIndices ? opengl::GPUBuffer::INDEX_BUFFER : opengl::GPUBuffer::VERTEX_BUFFER} }; //TODO: change type of buffer based on settings
			_buffers.emplace_back(std::move(buf));
		}
		i32 indexOf(str_ref name) {
			for (auto& b : _buffers) {
				if (b._settings.glAttribName == name) {
					auto index = &b - _buffers.data();
					return static_cast<i32>(index);
				}
			}
			return -1;
		}
		GPUData& operator[](i32 index) {
			return _buffers[index];
		}
		auto begin() { return _buffers.begin(); }
		auto end() { return _buffers.end(); }
		const auto begin() const { return _buffers.begin(); }
		const auto end() const { return _buffers.end(); }
		auto size() const { return _buffers.size(); }
	private:
		std::vector<GPUData> _buffers = {};
	};

	
}