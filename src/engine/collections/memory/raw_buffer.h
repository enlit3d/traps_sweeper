   #pragma once

#include "src/basic_types.h"
#include <memory>
#include <array>
#include <algorithm>
//#include <iterator>
#include <initializer_list>
#include <new>

namespace engine::memory
{

	template <typename T>
	class ArrayView : NotCopyable {
	public:
		ArrayView(T* start, i64 elemCount)
			: _start(start), _end(start + elemCount)
		{}

		ArrayView& operator=(std::initializer_list<T> list) {
			if (count() != list.size()) {
				throw;
			}
			std::copy(list.begin(), list.end(), this->begin());
			return *this;
		}

		ArrayView& operator=(const ArrayView& other) {
			if (this->byteSize() != other.byteSize()){
				throw;
			}
			std::copy(other.begin(), other.end(), this->begin());
			return *this;
		}

		ArrayView& operator=(T* data) {
			auto c = count();
			std::copy(data, data + c, _start);
			/*
			for (auto i = 0; i < count(); ++i) {
				*(_start + i) = *(data + i);
			}
			*/
			return *this;
		}

		T& operator[](i64 index) {  
			if (!this->boundsCheck(index)) {
				throw;
			}
			return *(_start + index);
		}

		const T& operator[](i64 index) const {
			if (!this->boundsCheck(index)) {
				throw;
			}
			return *(_start + index);
		}

		i64 count() const { return _end - _start; }

		bool boundsCheck(i64 index) const {
#ifdef _DEBUG
			return index >= 0 && index < (_end - _start);
#endif
			return true;
		}

		size_t byteSize() const { return count() * sizeof(T); }
		T* data() { return _start; }

		/*
		class Iter {
			T* _ptr;
		public:
			Iter(T* ptr) :_ptr(ptr) {};
			Iter& operator++() { ++_ptr; return *this; }
			bool operator!=(const Iter& other) const { return _ptr != other._ptr; }
			T& operator*() { return *_ptr; }
			const T& operator*() const { return *_ptr; }

			using iterator_category = std::forward_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = T;
			using pointer = T*;
			using reference = T&;
		};*/

		T* begin() const { return  _start; }
		T* end() const { return _end; }
	private:
		T* _start;
		T* _end;
	};

	class RawView {
	public:
		RawView(byte* start, byte* end) : _start(start), _end(end) {};
		template <typename T>
		ArrayView<T> view() {
			T* start = std::launder(reinterpret_cast<T*>(_start));
			T* end = std::launder(reinterpret_cast<T*>(_end));
			auto elemCount = end - start;
			return ArrayView<T>(start, elemCount);
		}
		explicit operator bool() {
			return byteSize() > 0;
		}
		byte* data() { return _start; }
		size_t byteSize() const {
			return _end - _start;
		}
	private:
		byte* _start;
		byte* _end;
	};

	class RawMemory {
	public:
		RawMemory(size_t byteSize)
			: _bytes(std::make_unique<byte[]>(byteSize)), _end(_bytes.get() + byteSize)
		{}
		byte* data() {
			return _bytes.get();
		}
		const byte* data() const {
			return _bytes.get();
		}
		size_t size() const {
			return _end - _bytes.get();
		}

		RawView rawView(i64 byteIndex, i64 bytes) {
			return RawView(data() + byteIndex, data() + byteIndex + bytes);
		}

		template <typename T>
		ArrayView<T> view(i64 byteIndex, i64 elemCount) {
			auto start = this->data() + byteIndex;
			auto end = start + elemCount * sizeof(T);
			if (start < this->data() || end >= _end) {
				throw;
			}
			return ArrayView<T>{reinterpret_cast<T*>(start), elemCount};
		}

	private:
		std::unique_ptr<byte[]> _bytes;
		byte* _end; // TODO: should store as a size instead!
	};
	
	template <i32 Bytes>
	class RawBuffer {
	public:
		byte* data() {
			return _bytes.data();
		}
		RawView rawView(i64 byteIndex, i64 bytes) {
			return RawView(data() + byteIndex, data() + byteIndex + bytes);
		}
		auto size() const {
			return _bytes.size();
		}
	private:
		std::array<byte, Bytes> _bytes = {};
	};
};