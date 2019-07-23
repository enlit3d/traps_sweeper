#pragma once

namespace engine {
	/// code taken from LLVM: https://llvm.org/doxygen/STLExtras_8h_source.html
	/// and renamed to FnRef
	///
	///	Original docs:
	/// An efficient, type-erasing, non-owning reference to a callable. This is
	 /// intended for use as the type of a function parameter that is not used
	 /// after the function in question returns.
	 ///
	 /// This class does not own the callable, so it is not in general safe to store
	 /// a FnRef.
	template<typename Fn> class FnRef;

	template<typename Ret, typename ...Params>
	class FnRef<Ret(Params...)> {
		Ret(*callback)(intptr_t callable, Params ...params) = nullptr;
		intptr_t callable;

		template<typename Callable>
		static Ret callback_fn(intptr_t callable, Params ...params) {
			return (*reinterpret_cast<Callable*>(callable))(
				std::forward<Params>(params)...);
		}

	public:
		FnRef() = default;
		FnRef(std::nullptr_t) {}

		template <typename Callable>
		FnRef(Callable&& callable,
			typename std::enable_if<
			!std::is_same<typename std::remove_reference<Callable>::type,
			FnRef>::value>::type* = nullptr)
			: callback(callback_fn<typename std::remove_reference<Callable>::type>),
			callable(reinterpret_cast<intptr_t>(&callable)) {}

		Ret operator()(Params ...params) const {
			return callback(callable, std::forward<Params>(params)...);
		}

		operator bool() const { return callback; }
	};
}