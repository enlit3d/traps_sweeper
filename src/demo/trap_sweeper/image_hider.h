#pragma once

#include "src/engine/render/model/model.h"
#include "src/engine/render/mesh/mesh_gen.h"
#include "src/engine/render/api/draw.h"
#include "mine_sweeper.h"
#include "src/ext/random.h"

#include <vector>
namespace sweeper {
	using namespace engine;

	class HiddenImg {
		using MASK_GRID = Grid2D<char, 5, 6>;
	public:
		inline HiddenImg(render::DrawBatch& batch, gui::Rect bounds, std::vector<render::Texture>& lewds) :
			_srcPic(batch),
			_masks(batch),
			_bounds(bounds),
			_lewds(lewds)
		{
			auto mesh = render::mesh_gen::Plane();
			auto lewdMat = render::Material{};

			for (auto i = 0; i < MASK_GRID::WIDTH * MASK_GRID::HEIGHT; ++i) {
				_unlockOrder.push_back(i);
			}

			math::shuffle(_unlockOrder);

			lewdMat.textureInput(lewds[0]);
			_srcPic.attach(mesh, lewdMat);

			this->makeMasks(_masks);
			this->placeImgIntoBounds();
		}

		inline void setProgress(f32 progress) {
			auto end = (i32)(progress * MASK_GRID::WIDTH * MASK_GRID::HEIGHT);
			auto index = 0;
			for (auto& m : _masks) {
				m.matInst.setColor(_unlockOrder[index] < end ? render::Color{0, 0, 0, 0} : render::Color{0, 0, 0, 255});
				++index;
			}
		}

		inline void nextPic() {
			_curPicIndex = (_curPicIndex + 1) % _lewds.size();
			for (auto& m : _srcPic) {
				m.matInst.tryChangeTexture(0, _lewds[_curPicIndex]);
			}
			this->placeImgIntoBounds();
			setProgress(0.0f);
			math::shuffle(_unlockOrder);
		}

		inline void draw(render::BasicCamera& camera) {
			render::draw(_srcPic, camera);
			render::draw(_masks, camera);
		}

	private:
		inline void makeMasks(render::Model& model) {
			auto segmentW = 1.0f / MASK_GRID::WIDTH;
			auto segmentH = 1.0f / MASK_GRID::HEIGHT;

			for (auto i = 0; i < MASK_GRID::WIDTH * MASK_GRID::HEIGHT; ++i) {
				auto coord = MASK_GRID::indexToCoord(i);
				auto mesh = render::mesh_gen::Plane(segmentW, segmentH);
				auto mat = render::Material{};
				mat.modelData = render::mat::MODEL_DATA_USE;
				auto verts = mesh.positions();
				for (auto v = 0; v < verts.count(); v += 3) {
					verts[v] += -0.5f + (coord.x+0.5) * segmentW;
					verts[v+1] += -0.5f + (coord.y+0.5) * segmentH;
				}
				auto& matInst = model.attach(mesh, mat);
				matInst.setColor({ 0,0,0,255 });
			}
		}

		inline void placeImgIntoBounds() {
			auto& srcPic = _lewds[_curPicIndex];
			auto scale = 1.0f;
			if (srcPic.w() > _bounds.w || srcPic.h() > _bounds.h) {
				auto targetAspectRatio = (f32)_bounds.w / (f32)_bounds.h;
				auto srcAspectRatio = (f32)srcPic.w() / (f32)srcPic.h();
				if (srcAspectRatio > targetAspectRatio) { // too wide
					scale = (f32)_bounds.w / (f32)srcPic.w();
				}
				else { // too tall
					scale = (f32)_bounds.h / (f32)srcPic.h();
				}
			}

			auto& t = _masks.transforms;
			t.scl[0] = (f32)srcPic.w() * scale;
			t.scl[1] = (f32)srcPic.h() * scale;
			t.pos[0] = (f32)(_bounds.x + _bounds.w / 2);
			t.pos[1] = (f32)(_bounds.y + _bounds.h / 2);
			_masks.updateTransforms();
			_srcPic.transforms = t;
			_srcPic.updateTransforms();
		}

	private:
		render::Model _srcPic;
		render::Model _masks;
		gui::Rect _bounds;
		f32 _unlockProgress = 0.0f;
		i32 _curPicIndex = 0;
		std::vector<render::Texture>& _lewds;
		std::vector<i32> _unlockOrder = {};
	};

}