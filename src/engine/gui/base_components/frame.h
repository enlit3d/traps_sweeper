#pragma once
#include "src/basic_types.h"
#include "renderable.h"
#include "callbacks.h"

#include <vector>
#include <memory>

#include <cassert>

namespace engine::gui {
	class GuiSystem;
	class Frame;

	struct GuiState {
	public:
		GuiState(std::weak_ptr<Frame> defaultFrame);
		std::weak_ptr<Frame> hovered;
		std::weak_ptr<Frame> selected;
	};

	class Frame : public std::enable_shared_from_this<Frame> {
	public:
		Frame(Rect rect);
		virtual ~Frame() = default;

		virtual void setVis(bool vis);
		virtual bool isVis() const;

		template <typename Child, typename... Args>
		std::shared_ptr<Child> emplaceChild(Args&&... args) {
			auto child = std::make_shared<Child>(std::forward<Args>(args)...);
			this->appendChild(child);
			return child;
		}

		template <typename Renderable_T, typename... Args>
		std::shared_ptr<Renderable_T> emplaceRenderable(Args&&... args) {
			auto renderable = std::make_shared<Renderable_T>(std::forward<Args>(args)...);
			this->appendRenderable(renderable);
			return renderable;
		}

		inline void attachCallback(Callbacks::Type type, Callbacks::Callback&& callback) {
			_callbacks.attachCallback(type, std::move(callback));
		}
		inline void raiseEvent(Callbacks::Type type, Event evt) {
			if (_settings.interactive) {
				_callbacks.raiseEvent(type, evt);
			}
		}

		struct Settings {
			bool interactive = true;
		};

		inline Settings& settings() {
			return _settings;
		}

	protected:
		virtual void shiftBy(Vec2 pos);
		virtual std::reference_wrapper<Frame> findHoveredElem(std::reference_wrapper<Frame> curBestCondidate, Vec2 pos);

		virtual void eventCursorMove() {};
		virtual void eventCursorDown() {};
		virtual void eventCursorUp() {};
		virtual void eventHover() {};
		virtual void eventUnHover() {};
		//virtual void eventFocus() {};
		//virtual void eventUnFocus() {};

		void appendChild(std::shared_ptr<Frame> ptr);
		void appendRenderable(std::shared_ptr<Renderable> ptr);

	protected:
		struct InternalState {
			bool isVis = true;
			i32 nodeHeight = 0; // distance or number of layers from root
		};

		Rect _rect = {};
		Settings _settings = {};
		InternalState _state = {};
		std::vector<std::shared_ptr<Frame>> _children = {};
		std::vector<std::shared_ptr<Renderable>> _attachments = {};
		Callbacks _callbacks = {};

		friend class GuiSystem;
	};
	
	class GuiSystem : NotCopyOrMoveable {
	public:
		GuiSystem(Rect rect);

		inline operator std::shared_ptr<Frame>& () {
			return _frame;
		}

		template <typename Child, typename... Args>
		std::shared_ptr<Child> emplaceChild(Args... args) {
			return _frame->emplaceChild<Child>(std::forward<Args...>(args...));
		}

		inline void cursorMove(Vec2 pos) {
			Frame& found = _frame->findHoveredElem(std::ref(*_frame), pos).get();
			auto prevHovered = _state.hovered.lock();
				
			if (&found != prevHovered.get()) {
				if (prevHovered && prevHovered->settings().interactive) {
					prevHovered->eventUnHover();
					prevHovered->raiseEvent(Callbacks::UNHOVER, {});
				}
				
				found.eventHover();
				found.raiseEvent(Callbacks::HOVER, {});

				_state.hovered = found.weak_from_this();
			}
			
			found.eventCursorMove();
			found.raiseEvent(Callbacks::CURSOR_MOVE, {});
		}

		inline void cursorDown() {
			if (auto hovered = _state.hovered.lock()) {
				if (!hovered->settings().interactive) { return; }
				hovered->eventCursorDown();
				hovered->raiseEvent(Callbacks::CURSOR_DOWN, {});
				_state.selected = _state.hovered;
			}
		}

		inline void cursorUp() {
			if (auto selected = _state.selected.lock()) {
				if (!selected->settings().interactive) { return; }
				selected->eventCursorUp();
				selected->raiseEvent(Callbacks::CURSOR_UP, {});
				_state.selected = _frame;
			}
		}

	private:
		std::shared_ptr<Frame> _frame;
		GuiState _state; // this must be after _frame to ensure it is initialized after!
	};
}