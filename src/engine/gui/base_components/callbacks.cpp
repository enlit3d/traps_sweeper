#include "callbacks.h"

namespace engine::gui {
	void Callbacks::attachCallback(Type type, Callback&& callback)
	{
		_callbacks.emplace_back(CallbackAndType{
			type,
			std::move(callback)
		});
	}
	void Callbacks::raiseEvent(Type type, Event evt)
	{
		for (auto& c : _callbacks) {
			if (c.type == type) {
				c.callback(evt);
			}
		}
	}
}