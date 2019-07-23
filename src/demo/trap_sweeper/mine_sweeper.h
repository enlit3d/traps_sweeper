#pragma once
#include <array>
#include "src/engine/gui/simple_button.h"
#include "src/ext/math.h"
#include "src/ext/functional.h"

namespace sweeper {
	using namespace engine;
	/*
		A simple "dynamic" array with a fixed capacity, allocated on the stack
	*/
	template <typename T, i32 N>
	class SmallArray {
	public:
		void push_back(T value) {
			if (_count >= N) { throw; }
			_data[_count++] = value;
		}
		T& operator[](i32 index) {
			if (index < 0 || index >= _count) { throw; }
			return _data[_count];
		}
		const T& operator[](i32 index) const {
			if (index < 0 || index >= _count) { throw; }
			return _data[_count];
		}
		T* begin() {
			return &(_data[0]);
		}
		T* end() {
			return &(_data[_count]);
		}
		i32 count() const {
			return _count;
		}
	private:
		T _data[N] = {};
		i32 _count = 0;
	};


	struct Coord {
		i32 x;
		i32 y;
	};

	/*
		An owning data structure representing a 2D grid, the grid W and H are compile time constants to make integer / and % much more efficient
	*/
	template <typename T, i32 W, i32 H>
	class Grid2D {		
	public:
		static_assert(W > 0 && H > 0);
		constexpr static auto WIDTH = W;
		constexpr static auto HEIGHT = H;
		using Index = i32;
		T& operator[](Coord c) {
			return _data[this->gridToIndex(c)];
		}
		const T& operator[](Coord c) const {
			return _data[this->gridToIndex(c)];
		}

		T& operator[](Index i) {
			if (i < 0 || i >= W * H) { throw; }
			return _data[i];
		}

		const T& operator[](Index i) const {
			if (i < 0 || i >= W * H) { throw; }
			return _data[i];
		}

		// given a coord, returns all the neighboring coordinates, taking into consideration coordinates on grid edges and corners
		SmallArray<Coord, 8> neighbours(Coord c) const {
			// there are ways to do this more efficiently, but who cares for now
			auto r = SmallArray<Coord, 8>{};
			for (auto dy = -1; dy <= 1; ++dy) {
				for (auto dx = -1; dx <= 1; ++dx) {
					if (dy == 0 && dx == 0) { continue; }
					auto x = c.x + dx;
					auto y = c.y + dy;
					if (x >= 0 && x < W && y >= 0 && y < H) {
						r.push_back(Coord{ x, y });
					}
				}
			}
			return r;
		}

		static Index gridToIndex(Coord c) {
			auto i = c.y * W + c.x;
			if (i < 0 || i >= W * H) { throw; }
			return i;
		}
		static Coord indexToCoord(Index i) {
			if (i < 0 || i >= W * H) { throw; }
			return Coord{ i % W, i / W };
		}
		i32 size() const {
			return (i32)(W * H);
		}
		auto begin() {
			return _data.begin();
;		}
		auto end() {
			return _data.end();
		}

	private:
		std::array<T, W*H> _data = {};
	};

	/*Represents each cell in the mine field*/
	struct Cell {
		enum State {
			INITIAL,  
			EXPLORED, 
			MARKED,
			BOOM
		};
		State state = INITIAL;
		bool hasMine = false;
		i32 surroundingMines = 0;
	};

	template <i32 W, i32 H>
	class BoardState {
	public:
		void init(i32 numOfMines=10) {
			numOfMines = math::clamp(numOfMines, 1, W * H / 2);
			// init all the cells
			for (Cell& cell : _state) {
				cell = Cell{};
			}
			// spawn mines
			auto minesToSpawn = numOfMines;
			while (minesToSpawn > 0) {
				auto c = Coord{ math::randInt(0, W-1), math::randInt(0, H-1) };
				auto& cell = _state[c];
				if (!cell.hasMine) {
					cell.hasMine = true;
					--minesToSpawn;
				}
			}
			_hitMine = false;
			_totalMines = numOfMines;
			_markedMines = 0;
			_explored = 0;
			
		}
		void explore(Coord coord, bool initialClick = false) { // "mines" a cell
			if (_hitMine) { return; } // game over man
			auto& cell = _state[coord];
			if (cell.state != Cell::INITIAL) {
				if (cell.state == Cell::EXPLORED && initialClick) { // explore neighbour cells if clicking on a cell with # of mines == # of surrounding flags
					auto flags = numberOfFlags(coord);
					if (flags == cell.surroundingMines) {
						for (Coord c : _state.neighbours(coord)) {
							explore(c);
						}
					}
				}
				return; 
			}
			if (cell.hasMine) { // RIP
				cell.state = Cell::BOOM;
				_hitMine = true;
				return;
			}
			cell.state = Cell::EXPLORED;
			cell.surroundingMines = numberOfMines(coord);
			_explored++;
			if (cell.surroundingMines == 0) {
				// if there are no mines, explore all neighboring cells automatically
				for (Coord c : _state.neighbours(coord)) {
					explore(c);
				}
			}
		}
		void mark(Coord coord) { // "flags" a cell
			if (_hitMine) { return; } // game over man
			auto& cell = _state[coord];
			if (cell.state == Cell::INITIAL) {
				cell.state = Cell::MARKED;
				_markedMines++;
			}
			else if (cell.state == Cell::MARKED) {
				cell.state = Cell::INITIAL;
				_markedMines--;
			}
		}

		const Grid2D<Cell, W, H>& getState() const {
			return _state;
		}

		i32 getMinesLeft() const {
			return _totalMines - _markedMines;
		}
		f32 getProgress() const {
			return (f32)_explored / (f32)(W * H - _totalMines);
		}
	private:
		i32 numberOfMines(Coord coord) { // get number of mines surrounding the cell 
			auto r = 0;
			for (Coord c : _state.neighbours(coord)) {
				if (_state[c].hasMine) {
					r += 1;
				}
			}
			return r;
		}
		i32 numberOfFlags(Coord coord) { // get number of marked cells 
			auto r = 0;
			for (Coord c : _state.neighbours(coord)) {
				if (_state[c].state == Cell::MARKED) {
					r += 1;
				}
			}
			return r;
		}
		Grid2D<Cell, W, H> _state = {};
		bool _hitMine = false;
		i32 _totalMines = 0;
		i32 _markedMines = 0;
		i32 _explored = 0;

		template <i32 W, i32 H>
		class BoardButtons;
	};

	template <i32 W, i32 H>
	class BoardButtons {
		using Callback_T = FnRef<void(Coord)>;
	public:
		BoardButtons(gui::Factory& fac, std::shared_ptr<gui::Frame>& parent, gui::Vec2 offset, Callback_T callback = {}) {
			// some hard coded values...
			using namespace gui;
			auto btnSize = Vec2{ 24, 24 };
			auto gap = Vec2{ 2, 2 };

			for (auto i = 0; i < _elems.size(); ++i) {
				auto coord = _elems.indexToCoord(i);
				auto x = offset.x + gap.x + (btnSize.x + gap.x) * coord.x;
				auto y = offset.y + gap.y + (btnSize.y + gap.y) * coord.y;
				_elems[coord] = fac.makeSimpleButton(
					parent,
					gui::Rect{ x, y, btnSize.x, btnSize.y },
					"",
					Color{ 0, 0, 0, 255 },
					Color{ 196, 196, 32, 255 },
					Color{ 128, 128, 128, 255 },
					[=]() {
						if (callback) {
							callback(coord);
						}
					}
				);
			}
		}

		void refresh(BoardState<W,H>& board) { // "redraws" the board
			auto& state = board.getState();
			for (auto i = 0; i < _elems.size(); ++i) {
				Cell cell = state[i];
				gui::Factory::SimpleButton& elem = _elems[i];
				switch (cell.state) {
				case cell.EXPLORED:
					elem.btn->settings().interactive = false;
					elem.btn->setState(gui::Button::State::SELECTED);
					if (cell.surroundingMines == 0) {
						elem.label->setText("");
					}
					else {
						constexpr gui::Color COLOR_LUT[8] = { // Look up table for the color of the number based on how many surrounding mines are there
							gui::Color{196, 196, 255, 255}, //1 surrounding mine
							gui::Color{164, 255, 164, 255}, //2 ...
							gui::Color{255, 164, 164, 255}, //3
							gui::Color{255, 255, 196, 255}, //4
							gui::Color{255, 255, 255, 255}, //5
							gui::Color{255, 255, 255, 255}, //6
							gui::Color{255, 255, 255, 255}, //7
							gui::Color{255, 255, 255, 255}  //8
						};

						elem.label->textSettings().color = COLOR_LUT[cell.surroundingMines-1];
						elem.label->setText(std::to_string(cell.surroundingMines));
					}
					
					break;
				case cell.BOOM:
					elem.btn->settings().interactive = false;
					elem.btn->setState(gui::Button::State::HOVERED);
					elem.label->textSettings().color = { 255,0,0,255 };
					elem.label->setText("*");
					break;
				case cell.MARKED:
					elem.btn->settings().interactive = true;
					elem.btn->setState(gui::Button::State::NORMAL);
					elem.label->textSettings().color = { 255,0,0,255 };
					elem.label->setText("!!");
					break;
				case cell.INITIAL:
					elem.btn->settings().interactive = true;
					elem.btn->setState(gui::Button::State::NORMAL);
					elem.label->setText("");
					break;
				default:
					break;
				}
			}
		}

		void setCallback(Callback_T callback) {
			for (auto i = 0; i < _elems.size(); ++i) {
				auto coord = _elems.indexToCoord(i);
				gui::Factory::SimpleButton& elem = _elems[coord];
				elem.btn->setCallback([=]() {
					if (callback) {
						callback(coord);
					}
				});
			}
		}
	private:
		Grid2D<gui::Factory::SimpleButton, W, H> _elems;
	};
}