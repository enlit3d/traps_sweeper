#pragma once

#include "src/basic_types.h"
#include "src/engine/collections/memory/raw_buffer.h"
#include <memory>
#include <vector>

namespace engine::render
{

	class MemBlock : NotCopyOrMoveable {
	public:
		class Handle {
		public:
			Handle(MemBlock& memblock) : _block(&memblock) {};
			~Handle() {
				if (_block) {
					_block->freeUse(*this);
				}
			}
			Handle& operator=(Handle&& other) noexcept {
				std::swap(_block, other._block);
				return *this;
			}
			Handle(Handle&& other) noexcept {
				std::swap(_block, other._block);
			};
			MemBlock* operator->() {
				return _block;
			}
		private:
			MemBlock* _block = nullptr;
		};
		static_assert(std::is_nothrow_move_assignable_v<Handle>);
		static_assert(!std::is_copy_assignable_v<Handle>);

	public:
		memory::RawView	req(i32 bytes) {
			if (!this->hasRoom(bytes)) {
				return { nullptr, nullptr };
			}
			else {
				auto v = _buffer.rawView(_head, bytes);
				_head += bytes;
				return v;
			}
		}
		bool hasRoom(i32 bytes) {
			return _head + bytes <= _buffer.size();
		}
		Handle newUser() {
			_useCount++;
			return Handle{ *this };
		}
	private:
		i32 _useCount = 0;
		i32 _head = 0;
		memory::RawBuffer<1024 * 1024> _buffer = {};
	private:
		friend class Handle;
		void freeUse(const Handle& handle) {
			_useCount--;
			if (_useCount == 0) {
				_head = 0;
			}
		}
	};

	
	class ManagedMemory {
	public:
		MemBlock::Handle reqBlock(i32 bytes) {
			for (auto& b : _memBlocks) {
				if (b->hasRoom(bytes)) {
					return b->newUser();
				}
			}
			auto& b = _memBlocks.emplace_back(std::make_unique<MemBlock>());
			return b->newUser();
		}
	private:
		std::vector<std::unique_ptr<MemBlock>> _memBlocks = {};
	};





}