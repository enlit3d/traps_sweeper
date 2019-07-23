#pragma once
#include <vector>

namespace engine::gui {
	template <typename T>
	class Handle {
		using Index = i32;
		using Id_T = i32;
	public:
		Handle() :
			_index(0), _id(0)
		{}

		explicit operator bool() const {
			return _id != 0 && ADDR_TABLE[_index].id == _id;
		}

		T* ptr() const {
			if (*this) {
				return { ADDR_TABLE[_index].addr };
			}
			else {
				return nullptr;
			}
		}

		friend bool operator==(const Handle& a, const Handle& b) {
			return a._index == b._index && a._id == b._id;
		}

		friend bool operator!=(const Handle& a, const Handle& b) {
			return !(a == b);
		}

	private:
		Handle(Index index, Id_T id) :
			_index(index), _id(id)
		{}

		static Handle acquire(T* addr) {
			auto id = NEXT_ID++;
			if (FREE_INDICES.size() == 0) {
				auto index = static_cast<Index>(ADDR_TABLE.size());
				ADDR_TABLE.push_back({ id, addr });
				return Handle{ index, id };
			}
			else {
				auto index = FREE_INDICES.back();
				FREE_INDICES.pop_back();
				ADDR_TABLE[index] = { id, addr };
				return Handle{ index, id };
			}
		}

		static void dispose(Handle h) {
			if (!h) { return; }
			ADDR_TABLE[h._index].id = 0;
			FREE_INDICES.push_back(h._index);
		}

		static void updateHandle(Handle& h, T* newAddr) {
			if (!h) { throw; }
			ADDR_TABLE[h._index].addr = newAddr;
		}

		template <typename T>
		friend class EnableHandle;

	private:
		Index _index;
		Id_T _id;
	private:
		struct Record {
			Id_T id;
			T* addr;
		};
		static inline std::vector<Record> ADDR_TABLE = {};
		static inline std::vector<Index> FREE_INDICES = {};
		static inline Index NEXT_ID = 1;

	public:
		static std::vector<Record>& DEBUG_getAddrTable() {
			return ADDR_TABLE;
		}

		static bool DEBUG_testAddrTable() {
			for (auto i = 0; i < ADDR_TABLE.size(); ++i) {
				auto r = ADDR_TABLE[i];
				if (r.id == 0) {
					if (!algo::has(FREE_INDICES, i)) { throw; }
				}
				else {
					auto& elem = *r.addr;
					auto h = elem.handle();
					auto rcd = ADDR_TABLE[h._index];
					if (rcd.id != h._id || rcd.addr != &elem) { throw; }
				}
			}

			return true;
		}
	};

	template <typename T>
	class EnableHandle {
	public:
		Handle<T> handle() const {
			return _handle;
		}

		EnableHandle() :
			_handle(Handle<T>::acquire(static_cast<T*>(this)))
		{
			static_assert(std::is_base_of_v<EnableHandle, T>);
		}

		~EnableHandle() {
			Handle<T>::dispose(_handle);
		}

		EnableHandle(EnableHandle&& other) noexcept :
			_handle(other._handle)
		{
			Handle<T>::updateHandle(_handle, static_cast<T*>(this));
			other._handle._id = 0;
		}

		EnableHandle& operator=(EnableHandle&& other) noexcept {
			Handle<T>::updateHandle(other._handle, static_cast<T*>(this));
			std::swap(_handle, other._handle);
			return *this;
		}

		// copy ctor may cause unexpected behaviour, therefore explicit .cloned should be used to create a copy instead
		EnableHandle clone() const {
			return EnableHandle{};
		}
		EnableHandle(const EnableHandle& other) = delete;
		EnableHandle& operator=(const EnableHandle& other) = delete;
	private:
		Handle<T> _handle;
	};
}