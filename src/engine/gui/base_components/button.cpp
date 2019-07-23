#include "button.h"

namespace engine::gui {
	Button::Button(
		Rect rect, 
		Callback_T&& callback,
		std::unique_ptr<Renderable>&& normalState,
		std::unique_ptr<Renderable>&& hoveredState, 
		std::unique_ptr<Renderable>&& selectedState
	) :
		Frame(rect),
		_callback{ std::move(callback) }
	{
		appendRenderable(std::move(normalState)); // index = State::NORMAL
		appendRenderable(std::move(hoveredState)); // index = State::HOVERED
		appendRenderable(std::move(selectedState)); // index = State::SELECTED

		updateBtn();
	}
	void Button::eventHover() {
		if (_state == State::NORMAL) {
			_state = State::HOVERED;
			this->updateBtn();
		}
	}
	void Button::eventUnHover() {
		if (_state != State::NORMAL) {
			_state = State::NORMAL;
			this->updateBtn();
		}
	}
	void Button::eventCursorDown() {
		_state = State::SELECTED;
		this->updateBtn();
	}
	void Button::eventCursorUp() {
		if (_state == State::SELECTED) {
			_state = State::HOVERED;
			_callback();
			this->updateBtn();
		}
	}
	void Button::updateBtn() {
		switch (_state) {
		case State::NORMAL:
			_attachments[(i32)State::NORMAL]->setVis(true);
			_attachments[(i32)State::HOVERED]->setVis(false);
			_attachments[(i32)State::SELECTED]->setVis(false);
			break;
		case State::HOVERED:
			_attachments[(i32)State::NORMAL]->setVis(false);
			_attachments[(i32)State::HOVERED]->setVis(true);
			_attachments[(i32)State::SELECTED]->setVis(false);
			break;
		case State::SELECTED:
			_attachments[(i32)State::NORMAL]->setVis(false);
			_attachments[(i32)State::HOVERED]->setVis(false);
			_attachments[(i32)State::SELECTED]->setVis(true);
			break;
		default:throw;
		}
	}
}