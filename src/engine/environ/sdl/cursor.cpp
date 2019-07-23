#include "cursor.h"
#include "main_loop.h"
#include <SDL_events.h>
#include <vector>
#include "env_events.h"


namespace engine::sdl
{
	struct Cursor::Impl {
		enum class EventType { CURSOR_DOWN, CURSOR_UP, CURSOR_MOVE, CURSOR_WHEEL };
		struct EventCallback {
			EventType type;
			Callback callback;
		};

		void doCallbacks(const Cursor& cursor, EventType type) {
			for (auto& c : callbacks) {
				if (c.type == type) {
					c.callback(cursor);
				}
			}
		}
		void pushCallback(EventType type, Callback&& callback) {
			callbacks.push_back({ type, std::move(callback) });
		}

		std::vector<EventCallback> callbacks = {};
	};

	Cursor::Button SDLBtnMap(decltype(SDL_MouseButtonEvent::button) button) {
		switch (button) {
		case SDL_BUTTON_LEFT:
			return Cursor::Button::LEFT;
		case SDL_BUTTON_RIGHT:
			return Cursor::Button::RIGHT;
		case SDL_BUTTON_MIDDLE:
			return Cursor::Button::MIDDLE;
		default:
			return Cursor::Button::NONE;
		}
	}

	Cursor::Cursor(MainLoop & loop)
		:_impl(std::make_unique<Impl>())
	{
		EnvEvents& evt = loop.manageEvents();
		evt.attachMouseEventCallback([this](SDL_Event& e) {
			switch (e.type) {
			case SDL_MOUSEBUTTONDOWN:
				this->button = SDLBtnMap(e.button.button);
				_impl->doCallbacks(*this, Impl::EventType::CURSOR_DOWN);
				break;
			case SDL_MOUSEBUTTONUP:
				this->button = SDLBtnMap(e.button.button);
				_impl->doCallbacks(*this, Impl::EventType::CURSOR_UP);
				break;
			case SDL_MOUSEMOTION:
				this->dx = e.motion.xrel;
				this->dy = e.motion.yrel;
				this->x = e.motion.x;
				this->y = e.motion.y;
				_impl->doCallbacks(*this, Impl::EventType::CURSOR_MOVE);
				break;
			case SDL_MOUSEWHEEL:
				// not implemented
				break;
			}
		});
	}
	Cursor::~Cursor() = default;
	void Cursor::onDown(Callback && c)
	{
		_impl->pushCallback(Impl::EventType::CURSOR_DOWN, std::move(c));
	}
	void Cursor::onUp(Callback && c)
	{
		_impl->pushCallback(Impl::EventType::CURSOR_UP, std::move(c));
	}
	void Cursor::onMove(Callback && c)
	{
		_impl->pushCallback(Impl::EventType::CURSOR_MOVE, std::move(c));
	}
	void Cursor::onWheel(Callback && c)
	{
		_impl->pushCallback(Impl::EventType::CURSOR_WHEEL, std::move(c));
	}
}