#pragma once
#include "src/basic_types.h"
#include "src/engine/collections/memory/raw_buffer.h"
#include "../../opengl/gl_texture.h"

#include "src/engine/utils/math.h"
#include "src/engine/utils/console.h"
#include <vector>
#include <array>
#include <unordered_map>

namespace engine::render
{
	template <typename T, i32 ElemsPerSlot>
	class DataTexture {
		static_assert(ElemsPerSlot >= 4);
	public:
		using Elem_T = T;
		using Index_T = i32;
		using SizeFactor_T = i32;
		struct Slot {
			Index_T index = -1;
			SizeFactor_T sizeFactor = 1;
		};

		struct DataSegment {
			Slot slot;
			std::array<Elem_T, ElemsPerSlot> data;
		};

	public:
		DataTexture(i32 initCapacity = 32)
		{
			auto& params = _texture.params(true);
			params.dataType = DataType::fromT<T>();
			params.format = params.RGBA;
			params.minFilter = params.NEAREST;
			params.magFilter = params.NEAREST;
			resize(initCapacity);
		}
		Slot getSlot(SizeFactor_T sizeFactor = 1) {
			_count++;
			if (_recycled.count(sizeFactor) > 0) {
				auto& recycledIndices = _recycled[sizeFactor];
				if (recycledIndices.size() > 0) {
					auto index = recycledIndices.back();
					recycledIndices.pop_back();
					return Slot{ index, sizeFactor };
				}
			}
			
			if (_highestIndex+sizeFactor > _capacity) {
				this->resize(_capacity * 2);
			}

			auto index = _highestIndex;
			_highestIndex+=sizeFactor;
			return Slot{ index, sizeFactor };
		}
		void freeSlot(Slot slot) {
			_count--;
			if (_recycled.count(slot.sizeFactor) <= 0) {
				_recycled[slot.sizeFactor] = {};
			}
			auto& recycledIndices = _recycled[slot.sizeFactor];
			recycledIndices.push_back(slot.index);
		}

		memory::ArrayView<T> getView(Slot slot, bool) {
			if (slot.index < 0 || slot.index >= _capacity) { throw; }
			_dirty = true;
			constexpr auto bytesPerElem = sizeof(T) * ElemsPerSlot;
			return memory::ArrayView<T>{reinterpret_cast<T*>(_bytes.data() + slot.index * bytesPerElem), ElemsPerSlot*slot.sizeFactor};
		}
		/*
		const memory::ArrayView<T> getView(Slot slot) const {
			if (slot.index < 0 || slot.index >= _capacity) { throw; }
			constexpr auto bytesPerElem = sizeof(T) * ElemsPerSlot;
			T* ptr = reinterpret_cast<T*>(const_cast<T*>(_bytes.data() + slot.index * bytesPerElem)); // TODO: would this const_cast cause problems?
			return memory::ArrayView<T>{ptr, ElemsPerSlot*slot.sizeFactor};
		}*/

		opengl::GPUTexture& texture() {
			this->upload();
			return _texture;
		}

		void upload() {
			if (_dirty) {
				if (_needResizeTexture) {
					f32 totalPixels = ElemsPerSlot * _capacity / 4.0f;
					f32 dim = math::sqrt(totalPixels);
					f32 w = static_cast<f32>(math::toNextPowOf2(math::toInt(math::max(dim, ElemsPerSlot / 4.0f))));
					f32 h = math::ceil(totalPixels / w);
					_texture.resizeAndClear(math::toInt(w), math::toInt(h));
					//console::out("texture resized to: " + std::to_string(w) + " " + std::to_string(h));
					_needResizeTexture = false;
				}
				_texture.write(_bytes.data());
				_dirty = false;
			}
			return;
		}

		operator opengl::GPUTexture&() {
			return this->texture();
		}
		auto capacity() const {
			return _capacity;
		}
		auto count() const {
			return _count;
		}
	private:
		void resize(i32 newCapacity) {
			if (newCapacity <= _capacity) { return; }
			constexpr auto bytesPerElem = sizeof(T) * ElemsPerSlot;
			_bytes.resize(bytesPerElem * newCapacity);

			_capacity = newCapacity;
			_dirty = true;
			_needResizeTexture = true;
		}

	private:
		opengl::GPUTexture _texture = {};
		std::vector<byte> _bytes = {};
		std::unordered_map<SizeFactor_T, std::vector<Index_T>> _recycled = {};
		i32 _capacity = 0;
		i32 _count = 0;
		i32 _highestIndex = 0;
		bool _dirty = false;
		bool _needResizeTexture = false;
		
	};
}