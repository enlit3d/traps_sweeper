#pragma once

#include <optional>

namespace engine {
	template <typename T>
	class Optional : private std::optional<T> 
	{
		using Parent = std::optional<T>;
	public:
		using Parent::optional;
		using Parent::operator*;
		using Parent::operator->;
		using Parent::operator=;
		using Parent::has_value;
		using Parent::operator bool;
		using Parent::value;
		using Parent::value_or;
		using Parent::swap;
		using Parent::reset;
		using Parent::emplace;
	};
	
	template <typename T>
	class Optional<T&> {
	public:
		Optional() {};
		Optional(T& v) 
			: _ptr(&v)
		{}
		T& get() {
			return *_ptr;
		}
		T& operator*() {
			return this->get();
		}
		T* operator->() {
			return &this->get();
		}
		explicit operator bool() const { return _ptr != nullptr; }
	private:
		T* _ptr = nullptr;
	};
}