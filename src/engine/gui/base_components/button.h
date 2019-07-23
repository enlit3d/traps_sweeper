#pragma once
#include "frame.h"

#include <functional>

namespace engine::gui {
	class Button : public Frame {
	public:
		enum class State : i32 {
			NORMAL = 0, HOVERED = 1, SELECTED = 2
		};
		using Callback_T = std::function<void()>;

		Button(
			Rect rect,
			Callback_T&& callback,
			std::unique_ptr<Renderable>&& normalState,
			std::unique_ptr<Renderable>&& hoveredState,
			std::unique_ptr<Renderable>&& selectedState
		);

		inline void setCallback(Callback_T callback){
			_callback = callback;
		}
		inline void setState(State state) {
			_state = state;
			updateBtn();
		}

	protected:
		void eventHover() override;
		void eventUnHover() override;
		void eventCursorDown() override;
		void eventCursorUp() override;

	private:
		void updateBtn();
	private:
		Callback_T _callback;
		State _state = State::NORMAL;
	};
}