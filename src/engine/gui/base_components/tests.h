#pragma once

#include "intrusive_handle.h"

void test() {
	struct T : public EnableHandle<T> {};
	auto& adr = Handle<T>::DEBUG_getAddrTable();

	{


		auto obj = T{};
		auto h = obj.handle();
		assert(h.ptr() == &obj);
		assert(Handle<T>::DEBUG_testAddrTable());

		// test copying
		auto copy = obj.clone();
		auto h_copied = copy.handle();
		assert(h_copied != h);
		assert(&copy != &obj);
		assert(h.ptr() == &obj);
		assert(h_copied.ptr() == &copy);
		assert(Handle<T>::DEBUG_testAddrTable());

		// test moving
		auto moved = std::move(copy);
		auto h_moved = moved.handle();
		assert(&moved != &copy);
		assert(h_moved.ptr() == &moved);
		assert(h_copied.ptr() == &moved);
		assert(Handle<T>::DEBUG_testAddrTable());

		// test move assign
		auto a = T{};
		auto b = T{};
		auto h1 = a.handle();
		auto h2 = b.handle();
		auto aaa = h2;

		assert(h1.ptr() == &a);
		assert(h2.ptr() == &b);

		a = std::move(b);
		assert(h1.ptr() == &a);
		assert(h2.ptr() == &a);

		// test handle copying & moving
		{
			auto h_copy = h;
			auto h_move = std::move(h);
			assert(h_copy == h);
			assert(h_move == h);
		}

		// test obj deletion
		{
			auto h = Handle<T>{};
			{
				auto to_be_removed = T{};
				h = to_be_removed.handle();
				assert(h.ptr() == &to_be_removed);
			}
			assert(!h);
			assert(Handle<T>::DEBUG_testAddrTable());
		}
	}

	{
		// testing handle tracking through moves and etc

		struct Thing : EnableHandle<Thing> {
			Thing(i32 v) :
				value(v)
			{}

			i32 value = 0;
		};
		static_assert(std::is_nothrow_move_constructible_v<Thing>);
		static_assert(std::is_nothrow_move_assignable_v<Thing>);
		auto& adr = Handle<Thing>::DEBUG_getAddrTable();

		constexpr auto VALUE = 42;

		auto h = Handle<Thing>{};
		assert(!h);
		{
			auto o = Thing{ VALUE };
			h = o.handle();
			assert((*h.ptr()).value == VALUE);
			assert(h.ptr() == &o);
			std::vector<Thing> c = {};
			{
				auto o2 = Thing{ 2 };
				o2 = std::move(o);
				assert((*h.ptr()).value == VALUE);
				assert(h.ptr() == &o2);
				c.push_back(std::move(o2));
			}

			assert(c.capacity() == 1);
			assert((*h.ptr()).value == VALUE);
			assert(h.ptr() == &c[0]);

			c.push_back(Thing{ 0 });
			c.push_back(Thing{ 0 });
			c.push_back(Thing{ 0 });
			assert(&o != &c[0]);
			assert((*h.ptr()).value == VALUE);
			assert(h.ptr() == &c[0]);

			auto c2 = std::move(c);
			assert((*h.ptr()).value == VALUE);
			assert(h.ptr() == &c2[0]);
			std::swap(c2[0], c2[3]);
			assert((*h.ptr()).value == VALUE);
			assert(h.ptr() == &c2[3]);
			c2.erase(c2.begin(), c2.begin() + 3);
			assert((*h.ptr()).value == VALUE);
			assert(h.ptr() == &c2[0]);
		}
		assert(!h);

		auto x = 10;
	}



	// deleted all previous objs here
	assert(Handle<T>::DEBUG_testAddrTable());
	{
		// test creation after deletion
		auto obj = T{};
		auto h = obj.handle();
		assert(h.ptr() == &obj);
		assert(Handle<T>::DEBUG_testAddrTable());

		// test copying after deletion
		auto copy = obj.clone();
		auto h_copied = copy.handle();
		assert(h_copied != h);
		assert(h.ptr() == &obj);
		assert(h_copied.ptr() == &copy);
		assert(Handle<T>::DEBUG_testAddrTable());

		// test moving after deletion
		auto moved = std::move(copy);
		auto h_moved = moved.handle();
		assert(&moved != &copy);
		assert(h_moved.ptr() == &moved);
		assert(h_copied.ptr() == &moved);
		assert(Handle<T>::DEBUG_testAddrTable());
	}
	assert(Handle<T>::DEBUG_testAddrTable());
	{
		// test batch creation & moving within a container
		constexpr auto TOTAL_OBJS = 64;
		constexpr auto TOTAL_OBJS_TO_REMOVE = 4;

		auto objs = std::vector<T>{};
		auto handles = std::vector<Handle<T>>{};
		for (auto i = 0; i < TOTAL_OBJS; ++i) {
			objs.push_back(T{});
			handles.push_back(objs[i].handle());
			auto x = 10;
		}
		for (auto i = 0; i < TOTAL_OBJS; ++i) {
			assert(handles[i].ptr() == &objs[i]);
		}
		assert(Handle<T>::DEBUG_testAddrTable());

		// pop a couple of objs from the vector in the front, testing move assignment & deletion
		objs.erase(objs.begin(), objs.begin() + TOTAL_OBJS_TO_REMOVE);
		assert(Handle<T>::DEBUG_testAddrTable());
		for (auto i = 0; i < TOTAL_OBJS_TO_REMOVE; ++i) {
			assert(!handles[i]);
		}
		for (auto i = TOTAL_OBJS_TO_REMOVE; i < TOTAL_OBJS; ++i) {
			assert(handles[i].ptr() == &objs[i - TOTAL_OBJS_TO_REMOVE]);
		}

	}
}