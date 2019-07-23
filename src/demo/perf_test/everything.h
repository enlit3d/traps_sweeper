#pragma once
#include "src/ext/math.h"
#include "src/engine/render/model/model.h"
#include <vector>


engine::render::MeshData createPlane(f32 w = 1.0f, f32 h = 1.0f) {
	auto mesh = engine::render::MeshData(4, 6);
	auto hW = w / 2.0f;
	auto hH = h / 2.0f;
	mesh.positions() = {
		hW, hH, 0.0f,
		-hW, hH, 0.0f,
		-hW, -hH, 0.0f,
		hW, -hH, 0.0f
	};
	mesh.uvs() = {
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f
	};
	mesh.indices() = {
		0, 1, 2,
		0, 2, 3
	};

	return mesh;
}

engine::render::MeshData createCube() {
	auto mesh = engine::render::MeshData(24, 36);
	mesh.positions() = {
		0.5, 0.5f, 0.5f,  -0.5f, 0.5f, 0.5f,  -0.5f,-0.5f, 0.5f,   0.5f,-0.5f, 0.5f, // v0-v1-v2-v3 front
		0.5f, 0.5f, 0.5f,   0.5f,-0.5f, 0.5f,   0.5f,-0.5f,-0.5f,   0.5f, 0.5f,-0.5f, // v0-v3-v4-v5 rig0.5ft
		0.5f, 0.5f, 0.5f,   0.5f, 0.5f,-0.5f,  -0.5f, 0.5f,-0.5f,  -0.5f, 0.5f, 0.5f, // v0-v5-v6-v1 up
		-0.5f, 0.5f, 0.5f,  -0.5f, 0.5f,-0.5f,  -0.5f,-0.5f,-0.5f,  -0.5f,-0.5f, 0.5f, // v1-v6-v7-v2 left
		-0.5f,-0.5f,-0.5f,   0.5f,-0.5f,-0.5f,   0.5f,-0.5f, 0.5f,  -0.5f,-0.5f, 0.5f, // v7-v4-v3-v2 0.5fo0.5fn
		0.5f,-0.5f,-0.5f,  -0.5f,-0.5f,-0.5f,  -0.5f, 0.5f,-0.5f,   0.5f, 0.5f,-0.5f  // v4-v7-v6-v5 back
	};

	mesh.uvs() = {
		1.0f, 1.0f,   0.0f, 1.0f,   0.0f, 0.0f,   1.0f, 0.0f, // v0.0f-v1-v2-v3 front
		0.0f, 1.0f,   0.0f, 0.0f,   1.0f, 0.0f,   1.0f, 1.0f, // v0.0f-v3-v4-v5 right
		0.0f, 1.0f,   0.0f, 0.0f,   1.0f, 0.0f,   1.0f, 1.0f, // v0.0f-v5-v6-v1 up
		1.0f, 1.0f,   0.0f, 1.0f,   0.0f, 0.0f,   1.0f, 0.0f, // v1-v6-v7-v2 left
		1.0f, 0.0f,   0.0f, 0.0f,   0.0f, 1.0f,   1.0f, 1.0f, // v7-v4-v3-v2 down
		0.0f, 0.0f,   1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f  // v4-v7-v6-v5 back
	};
	mesh.indices() = {
		0, 1, 2,   0, 2, 3,    // front
		4, 5, 6,   4, 6, 7,    // right
		8, 9,10,   8,10,11,    // up
		12,13,14,  12,14,15,    // left
		16,17,18,  16,18,19,    // down
		20,21,22,  20,22,23     // back
	};
	return mesh;
}

namespace demo {


	using namespace engine;
	using namespace matrix;
	using namespace render;

	struct Number {
		Number(f32 v) 
			: _v{v}
		{}
		Number(f64 v)
			: _v{ static_cast<f32>(v) }
		{}
		operator f32() { return _v; }
	private:
		f32 _v;
	};

	static auto SPAWNER_MESH = createPlane(5.0, 5.0);
	static auto DEBRIS_MESH = createPlane(2.0, 2.0);

	class Spawner {
	public:
		Spawner(DrawBatch& batch, Vec3 pos, Number ts) 
			: _model{batch, render::mat::TRANSFORM_POS_ONLY}
		{
			auto& mesh = SPAWNER_MESH;
			auto mat = Material{};
			mat.modelData = mat::MODEL_DATA_USE;

			auto& matInst = _model.attach(mesh, mat);
			matInst.setColor( engine::render::Color{ (u8)math::randBits(),(u8)math::randBits(),(u8)math::randBits(),255 }); // TODO: change this a nicer API

			_model.transforms.pos = pos;
			_model.transforms.scl *= 5.0f;
			_model.setTransform();

			_vel[0] = math::randFloat(-1.0f, 1.0f);
			_vel[1] = math::randFloat(-1.0f, 1.0f);
			_vel.setLength(math::randFloat(30.0f, 60.0f));

			_nextSpawnTS = ts + math::randFloat(0, SPAWN_INTERVAL);
		}

		bool update(Number dt, Number ts, const Vec3& forcePos) {
			auto& pos = _model.transforms.pos;
			pos += _vel * dt;
			/*
			auto diff = pos - forcePos;
			if (diff.lengthSqr() < 50 * 50 && diff.lengthSqr() > 0) {
				diff.setLength(_vel.length());
				_vel = diff;
			}
			*/
			if (pos[0] < 0) {
				_vel[0] = math::abs(_vel[0]);
			}
			else if (pos[0] > 800) {
				_vel[0] = -math::abs(_vel[0]);
			}
			if (pos[1] < 0) {
				_vel[1] = math::abs(_vel[1]);
			}
			else if (pos[1] > 600) {
				_vel[1] = -math::abs(_vel[1]);
			}
			_model.setTransform();
			if (_nextSpawnTS < ts) {
				_nextSpawnTS = ts + SPAWN_INTERVAL;
				return true;
			}
			return false;
		}
	private:
		friend class Demo;
		Model _model;
		Vec3 _vel = {};
		f32 _nextSpawnTS = 0.0;
		constexpr static auto SPAWN_INTERVAL = 0.2f;
	};
	
	class Debris {
	public:
		Debris(DrawBatch& batch, Vec3 pos)
			: _model{batch, render::mat::TRANSFORM_POS_ONLY}
		{
			auto& mesh = DEBRIS_MESH;
			auto mat = Material{};
			//mat.dynamic = mat::DYNAMIC_DATA_USE;
			_model.attach(mesh, mat);
			//auto& matInst = _model.attach(mesh, mat);
			//matInst.matDataView(true) = { (u8)math::randBits(),(u8)math::randBits(),(u8)math::randBits(),255 };

			_model.transforms.pos = pos;
			_model.transforms.scl *= 2.0f;
			_model.setTransform();

			_vel[0] = math::randFloat(-1.0f, 1.0f);
			_vel[1] = math::randFloat(-1.0f, 1.0f);

			_vel.setLength(math::randFloat(15.0f, 30.0f));
			_life = math::randFloat(3.0f, 6.0f);
			_startLife = _life;

		}
		void update(Number dt, Number ts) {
			_model.transforms.pos += _vel * dt;
			_model.setTransform();
			_life -= dt;
			//auto r = _life / _startLife;
			//auto& matInst = _model.getMesh(0).mat;

			//list.emplace_back(matInst.getWriteSegment());
			//list.back().data[0] = 255;
			//list.back().data[0] = 255;
			
			//matInst.matDataView(true)[0] = 0;
			//matInst.matDataView(true)[3] = (f32)255.0f*r;
		}

		bool shouldDie() const {
			return _life <= 0.0f;
		}
	private:
		Model _model;
		Vec3 _vel = {};
		f32 _life;
		f32 _startLife;
	};

	class Demo {
		constexpr auto static MAX_MODELS_PER_BATCH = 128*128;
		//constexpr auto static MAX_MODELS_PER_BATCH = 65536000;
	public:
		Demo(RenderContext& ctx)
			: _ctx{ctx}
		{
			batches.emplace_back(ctx);
		}
		void update(Number dt, Number ts) {
			constexpr auto remove = false;
			if constexpr(remove) {
				debris.erase(
					std::remove_if(
						debris.begin(),
						debris.end(),
						[](Debris& d) {return d.shouldDie(); })
					, debris.end()
				);
			}
			

			for (auto& s : spawners) {
				if (s.update(dt, ts, forcePos) && remove) {
					debris.emplace_back(nextBatch(), s._model.transforms.pos);
				}
			}

			for (auto& d : debris) {
				d.update(dt, ts);
			}
		}
		void spawnAction(Vec3 pos, f32 timestamp, i32 amt = 100) {
			while (amt-- > 0) {
				spawners.emplace_back(nextBatch(), pos, timestamp);
			}
		}
	private:
		DrawBatch& nextBatch() {
			if (batches.back().count() < MAX_MODELS_PER_BATCH) {
				return batches.back();
			}
			return batches.emplace_back(_ctx);
		}
	private:
		RenderContext& _ctx;

	public:
		std::vector<DrawBatch> batches = {};
		std::vector<Spawner> spawners = {};
		std::vector<Debris> debris = {};
		Vec3 forcePos = {};
		
	};
}