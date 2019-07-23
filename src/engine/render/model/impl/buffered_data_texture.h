#pragma once
#include "src/basic_types.h"
#include "src/engine/collections/memory/raw_buffer.h"
#include "../../opengl/gl_texture.h"
#include "src/engine/utils/math.h"

#include <array>
#include <vector>

#include "src/engine/utils/console.h"


namespace engine::render {
	template <typename T, i32 ElemsPerSlot, i32 Buffers = 3>
	class BufferedDataTexture {
	public:
		using Slot = i32;
		BufferedDataTexture(i32 initCapacity = 32) {
			resize(initCapacity);

			for (auto& t : _textures) {
				auto& params = t.params(true);
				params.dataType = DataType::fromT<T>();
				params.format = params.RGBA;
				params.minFilter = params.NEAREST;
				params.magFilter = params.NEAREST;

				t.resizeAndClear(1, 1);
			}
		}

		Slot getSlot() {
			if (_recycledIndices.size() > 0) {
				auto index = _recycledIndices.back();
				_recycledIndices.pop_back();
				return index;
			}

			if (_count >= _capacity) {
				this->resize(_capacity * 2);
			}

			auto index = _count;
			_count++;
			return index;
		}

		void freeSlot(Slot slot) {
			_recycledIndices.push_back(slot);
		}

		//typename DataTexture::DataSegment getWriteSegment() const {
		//	return _target->getWriteSegment(_slot);
		//}

		const memory::ArrayView<T> getView(Slot slot) const {
			if (slot < 0 || slot >= _capacity) { throw; }
			constexpr auto bytesPerElem = sizeof(T) * ElemsPerSlot;
			return memory::ArrayView<T>{reinterpret_cast<T*>(const_cast<T*>(_bytes.data() + slot * bytesPerElem)), ElemsPerSlot};
		}

		memory::ArrayView<T> getView(Slot slot, bool) {
			if (slot < 0 || slot >= _capacity) { throw; }
			constexpr auto bytesPerElem = sizeof(T) * ElemsPerSlot;
			return memory::ArrayView<T>{reinterpret_cast<T*>(_bytes.data() + slot * bytesPerElem), ElemsPerSlot};
		}

		opengl::GPUTexture& texture() {
			auto index = (_curWriteTarget + 1) % Buffers;
			auto& readTexture = _textures[index];
			
			//console::out("buffer read t: " + std::to_string(index));

			return readTexture;
		}

		void upload() {
			if (_count == 0) { return; }

			auto& writeTexture = _textures[_curWriteTarget];
			_curWriteTarget = (_curWriteTarget + 1) % Buffers;

			auto requiredByteSize = _capacity * ElemsPerSlot * sizeof(T);
			if (requiredByteSize > writeTexture.byteSize()) {
				f32 totalPixels = ElemsPerSlot * _capacity / 4.0f;
				f32 dim = math::sqrt(totalPixels);
				f32 w = static_cast<f32>(math::toNextPowOf2(math::toInt(math::max(dim, ElemsPerSlot / 4))));
				f32 h = math::ceil(totalPixels / w);
				writeTexture.resizeAndClear(math::toInt(w), math::toInt(h));
			}

			//console::out("buffer write t: " + std::to_string(_curWriteTarget));

			writeTexture.write(_bytes.data());
		}

	private:
		void resize(i32 newCapacity) {
			if (newCapacity <= _capacity) { return; }
			constexpr auto bytesPerElem = sizeof(T) * ElemsPerSlot;
			_bytes.resize(bytesPerElem * newCapacity);
			_capacity = newCapacity;
		}
	private:
		std::array<opengl::GPUTexture, Buffers> _textures = {};
		std::vector<byte> _bytes = {};
		std::vector<Slot> _recycledIndices = {};
		i32 _capacity = 0;
		i32 _count = 0;
		i32 _curWriteTarget = 0;
	};

	template <typename DataTexture_T>
	class ManagedTexture {
		using Slot = typename DataTexture_T::Slot;
		using Index_T = typename DataTexture_T::Index_T;
		using SizeFactor_T = typename DataTexture_T::SizeFactor_T;
	public:
		ManagedTexture()
			: _texture{ std::make_unique<DataTexture_T>() }
		{
		}

		class UniqueHandle {
		public:
			UniqueHandle() {}
			UniqueHandle(DataTexture_T& data, SizeFactor_T sizeFactor = 1)
				: _target(&data)
			{
				//TODO: perhaps initialize the target buffer?
				_slot = _target->getSlot(sizeFactor);
			}
			~UniqueHandle() {
				if (_target) {
					_target->freeSlot(_slot);
				}
			}
			UniqueHandle(UniqueHandle&& other) noexcept {
				(*this) = std::move(other);
			}
			UniqueHandle& operator=(UniqueHandle&& other) noexcept {
				std::swap(_target, other._target);
				std::swap(_slot, other._slot);
				return *this;
			}
			
			auto view(bool) {
				return _target->getView(_slot, true);
			}

			Index_T index() const {
				return _slot.index;
			}
			opengl::GPUTexture& texture() {
				return _target->texture();
			}
		private:
			DataTexture_T* _target = nullptr;
			Slot _slot;
		};

		UniqueHandle acquire(SizeFactor_T sizeFactor = 1) {
			return UniqueHandle{ *_texture , sizeFactor};
		}

		i32 count() const {
			return _texture->count();
		}

		opengl::GPUTexture& texture() {
			return _texture->texture();
		}

		void upload() {
			return _texture->upload();
		}
	private:
		std::unique_ptr<DataTexture_T> _texture = {}; // ptr to maintain memory stability, for things like if this object is placed into a non-stable container
	};
}