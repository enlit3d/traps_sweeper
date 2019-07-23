#pragma once

#include <functional>
#include <vector>

namespace engine::gui {

	struct Event { // TODO: determine what data needs to go into an event

	};

	class Callbacks {
	public:
		enum Type {
			NONE,
			CURSOR_MOVE,
			CURSOR_DOWN,
			CURSOR_UP,
			HOVER,
			UNHOVER
		};

		using Callback = std::function<void(Event& evt)>;

		void attachCallback(Type type, Callback&& callback); //TODO: return an CallbackId (like an index or similiar) to support deletion?
		void raiseEvent(Type type, Event evt);

	private:
		struct CallbackAndType {
			Type type = NONE;
			Callback callback;
		};
		std::vector<CallbackAndType> _callbacks;
	};

}