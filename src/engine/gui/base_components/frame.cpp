#include "frame.h"

namespace engine::gui {
	void Frame::setVis(bool vis)
	{
		if (vis == _state.isVis) { return; }
		for (auto& renderable : _attachments) {
			renderable->setVis(vis);
		}
		for (auto& child : _children) {
			child->setVis(vis);
		}
		_state.isVis = vis;
	}
	bool Frame::isVis() const
	{
		return _state.isVis;
	}

	void Frame::shiftBy(Vec2 pos)
	{
		_rect.x += pos.x;
		_rect.y += pos.y;

		for (auto& renderable : _attachments) {
			renderable->setBounds(_rect);
		}

		for (auto& child : _children) {
			child->shiftBy(pos);
		}
	}

	std::reference_wrapper<Frame> Frame::findHoveredElem(std::reference_wrapper<Frame> curBestCondidate, Vec2 pos)
	{
		if (!isVis()) { return curBestCondidate; }
		auto cursorInside = _rect.contains(pos);
		if (cursorInside && _settings.interactive) {
			//this->raiseEvent(Callbacks::CURSOR_MOVE, {});
			//eventCursorMove();

			//if (this->_state.nodeHeight >= curBestCondidate.get()._state.nodeHeight) {
			if (true){
				curBestCondidate = std::ref(*this);
			}

			for (auto& c : _children) {
				auto r = c->findHoveredElem(curBestCondidate, pos);
				if (&r.get() != &curBestCondidate.get()) {
					curBestCondidate = r;
				}
			}
		}
		return curBestCondidate;
	}

	void Frame::appendChild(std::shared_ptr<Frame> ptr) {
		auto& handle = _children.emplace_back(std::move(ptr));
		handle->shiftBy({ _rect.x, _rect.y });
		handle->_state.nodeHeight = this->_state.nodeHeight + 1;
	}

	void Frame::appendRenderable(std::shared_ptr<Renderable> ptr)
	{
		auto& handle = _attachments.emplace_back(std::move(ptr));
		handle->setBounds(_rect);
	}

	Frame::Frame(Rect rect)
		: _rect(rect)
	{}

	GuiSystem::GuiSystem(Rect rect)
		: _frame{ std::make_shared<Frame>(rect) }, _state(_frame)
	{
	}
	GuiState::GuiState(std::weak_ptr<Frame> defaultFrame)
	{
		hovered = defaultFrame;
		selected = defaultFrame;
	}
}