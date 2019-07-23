#include "keyboard.h"
#include "main_loop.h"
#include <SDL_events.h>
#include <SDL_keyboard.h>
#include <vector>
#include "env_events.h"

namespace engine::sdl
{
	struct Keyboard::Impl {
		enum class EventType { KEY_DOWN, KEY_UP };
		struct EventCallback {
			EventType type;
			Callback callback;
		};

		void doCallbacks(const Keyboard& cursor, EventType type) {
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

	Keyboard::Key SDLKeyMap(decltype(SDL_KeyboardEvent::keysym.sym) key) {
		using K = Keyboard::Key;
		switch (key) {
		case SDLK_0:
			return K::_0;
		case SDLK_1:
			return K::_1;
		case SDLK_2:
			return K::_2;
		case SDLK_3:
			return K::_3;
		case SDLK_4:
			return K::_4;
		case SDLK_5:
			return K::_5;
		case SDLK_6:
			return K::_6;
		case SDLK_7:
			return K::_7;
		case SDLK_8:
			return K::_8;
		case SDLK_9:
			return K::_9;
		case SDLK_a:
			return K::A;
		case SDLK_b:
			return K::B;
		case SDLK_c:
			return K::C;
		case SDLK_d:
			return K::D;
		case SDLK_e:
			return K::E;
		case SDLK_f:
			return K::F;
		case SDLK_g:
			return K::G;
		case SDLK_h:
			return K::H;
		case SDLK_i:
			return K::I;
		case SDLK_j:
			return K::J;
		case SDLK_k:
			return K::K;
		case SDLK_l:
			return K::L;
		case SDLK_m:
			return K::M;
		case SDLK_o:
			return K::O;
		case SDLK_p:
			return K::P;
		case SDLK_q:
			return K::Q;
		case SDLK_r:
			return K::R;
		case SDLK_s:
			return K::S;
		case SDLK_t:
			return K::T;
		case SDLK_u:
			return K::U;
		case SDLK_v:
			return K::V;
		case SDLK_w:
			return K::W;
		case SDLK_x:
			return K::X;
		case SDLK_y:
			return K::Y;
		case SDLK_z:
			return K::Z;
		case SDLK_SPACE:
			return K::SPACE;
		case SDLK_UP:
			return K::UP;
		case SDLK_DOWN:
			return K::DOWN;
		case SDLK_LEFT:
			return K::LEFT;
		case SDLK_RIGHT:
			return K::RIGHT;
		default:
			return K::DONT_KNOW;
		}
	};


	Keyboard::Keyboard(MainLoop & loop)
		: _impl(std::make_unique<Impl>())
	{
		auto& evt = loop.manageEvents();
		evt.attachKeyEventCallback([this](SDL_Event& e) {
			auto eventType = (e.key.state == SDL_PRESSED) ? Impl::EventType::KEY_DOWN : Impl::EventType::KEY_UP;
			this->key = SDLKeyMap(e.key.keysym.sym);
			_impl->doCallbacks(*this, eventType);
		});
	}

	Keyboard::~Keyboard() = default;

	void Keyboard::onDown(Callback && c)
	{
		_impl->pushCallback(Impl::EventType::KEY_DOWN, std::move(c));
	}

	void Keyboard::onUp(Callback && c)
	{
		_impl->pushCallback(Impl::EventType::KEY_UP, std::move(c));
	}

}