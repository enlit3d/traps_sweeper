#pragma once
#include <array>
#include <vector>
#include <memory>
#include <type_traits>
#include "src/basic_types.h"
#include "src/engine/utils/math.h"

namespace engine::collections
{
	template <typename T, i32 elemsPerBlock = 128>
	class Blocks {
		using Block = std::array<T, elemsPerBlock>;
	public:
		static_assert(math::isPowOf2<elemsPerBlock>(), "elemsPerBlock must be a power of 2!");
		Blocks() {};
		T& operator[] (int index) {
			auto blockIndex = math::fastDivPowOf2<elemsPerBlock>(index);
			auto elemIndex = index - blockIndex * elemsPerBlock;
			return (*_data[blockIndex])[elemIndex];
		}
		const T& operator[] (int index) const {
			auto blockIndex = math::fastDivPowOf2<elemsPerBlock>(index);
			auto elemIndex = index - blockIndex * elemsPerBlock;
			return (*_data[blockIndex])[elemIndex];
		}

		T& push() {
			auto blockIndex = math::fastDivPowOf2<elemsPerBlock>(_index);
			auto elemIndex = _index - blockIndex * elemsPerBlock;

			if (blockIndex >= _data.size()) {
				_data.emplace_back(std::make_unique<Block>());
				_index++;
				return (*_data.back())[0];
			}
			else {
				_index++;
				return (*_data[blockIndex])[elemIndex];
			}
		}

		i32 count() {
			return static_cast<i32>(_index);
		}
	private:
		std::vector<std::unique_ptr<Block>> _data = {};
		i32 _index = 0;
	};

	template <typename T, u32 N = 256>
	class HandleMapStable {
		static_assert(std::is_default_constructible_v<T>);
		struct DataSlot {
			u32 id = 0;
			T data;
		};
	public:
		//using UntypedHandle = impl::UntypedHandle;
		struct Handle {
			u32 slotIndex;
			u32 id;
		};
		const Handle acquire() {
			auto id = nextId++;
			++this->elemsCount;
			if (freeIndices.size() == 0) {
				auto slotIndex = static_cast<u32>(values.count());
				auto& slot = values.push();
				slot.id = id;
				return Handle{ slotIndex, id };
			}
			else {
				auto slotIndex = freeIndices.back();
				freeIndices.pop_back();
				values[slotIndex].id = id;
				return Handle{ slotIndex, id };
			}
		}

		const T& get(Handle h) const {
			if (!verify(h)) { std::terminate(); }
			return values[h.slotIndex].data;
		}

		T& get(Handle h) {
			if (!verify(h)) { std::terminate(); }
			return values[h.slotIndex].data;
		}

		void dispose(Handle h) {
			if (!verify(h)) { return; }
			values[h.slotIndex].id = 0;  // fails the validate test for handles that are pointing to the same slot
			freeIndices.push_back(h.slotIndex);
			--this->elemsCount;
		}

		bool verify(Handle h) const {
			return values[h.slotIndex].id == h.id;
		}
	private:
		Blocks<DataSlot, N> values = {};
		std::vector<u32> freeIndices = {};  // availiable indices stack
		u32 nextId = 1;  // starts at 1 so id = 0 is a null value
		i32 elemsCount = 0;
	};
}