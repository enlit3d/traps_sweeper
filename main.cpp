#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"

#pragma warning (push, 0)
#define GLEW_STATIC 
#include <GL/glew.h>

#include "fmt/format.h"
#include "SDL_Main.h"
#include "SDL.h"

#pragma warning (pop)
#pragma clang diagnostic pop

#include "src/engine/environ/sdl_environ.h"
#include "src/engine/environ/loader.h"
#include "src/engine/render/ctx/render_context.h"

#include "src/engine/render/scene/camera.h"
#include "src/engine/render/model/draw_batch.h"
#include "src/engine/render/model/model.h"

#include "src/engine/render/api/draw.h"
#include "src/engine/render/api/upload.h"

#include "src/engine/render/opengl/error_handling.h"

#include "src/ext/optional.h"
#include "src/engine/utils/console.h"

#include "src/engine/render/text/text_sprite.h"
#include "src/engine/render/text/glyph_texture_sheet.h"
#include "src/engine/render/text/freetype_font.h"
#include "src/engine/render/text/utf.h"

#include "src/engine/render/misc/fps_counter.h"
#include "src/engine/gui/gui.h"
#include "src/engine/gui/simple_button.h"

#include "src/demo/trap_sweeper/mine_sweeper.h"
#include "src/demo/trap_sweeper/image_hider.h"

#include <iostream>
#include <cassert>
#include <tuple>
#include <filesystem>

namespace fs = std::filesystem;

using namespace engine;

extern "C"
{
	// request dedicated graphics for nvidia
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	// request dedicated graphics for amd
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}


int main(int argc, char* argv[]) {

	auto window = env::Window{800, 600};
	auto ctx = render::RenderContext{};
	auto loader = env::Loader{};

	auto TRAP_PICS = std::vector<engine::render::Texture>{};

	{
		fs::path path = fs::current_path();
		path += fs::path("/assets/traps");
		path.make_preferred();
		if (fs::exists(path)) {
			for (const auto& entry : fs::directory_iterator(path)) {
				auto pathStr = entry.path().string();
				render::Texture t = loader.loadImg(pathStr);
				auto& params = t->params(true);
				params.magFilter = params.LINEAR;
				params.minFilter = params.LINEAR_MIPMAP_LINEAR;
				TRAP_PICS.push_back(std::move(t));
			}
		}
		if (TRAP_PICS.size() == 0) { // enforce non-empty
			auto tex = render::Texture{};
			auto& params = tex->params(true);
			params.format = render::opengl::TextureParams::Format::RGBA;
			tex->resizeAndClear(1,1);
			byte bytes[4] = { 0,0,0,0 };
			tex->write(bytes);
			TRAP_PICS.push_back(std::move(tex));
		}
	}


	auto camera = render::BasicCamera{};
	camera.projection(true).orthogonal(0, 800, 0, 600, -64, 64);

	auto guiCamera = render::BasicCamera{};
	guiCamera.projection(true).orthogonal(0, 800, 600, 0, -64, 64);

	using namespace render;
	using namespace render::mat;

	auto glyphs = GlyphTextureSheet{};

	
	auto batch = DrawBatch(ctx);
	auto disjointBatch = DrawBatch(ctx);
	auto textBatch = DrawBatch(ctx);
	auto guiBatch = DrawBatch(ctx);



	auto engFont = glyphs.loadFont("assets/english.ttf");
	

	auto sys = gui::GuiSystem{ {0,0,800,600} };
	


	auto loop = env::MainLoop();
	auto cursor = env::Cursor(loop);
	auto kb = env::Keyboard(loop);
	

	{

		std::string gfxCardInfo;
		{
			const GLubyte* renderer = glGetString(GL_RENDERER);
			if (renderer) {
				gfxCardInfo = reinterpret_cast<const char*>(renderer);
				// glGetString returns a pointer to a static string, no need to delete
			}
		}

		auto frame = sys.emplaceChild<gui::Frame>(gui::Rect{ 800-200,20,200,300 });
		auto bg = frame->emplaceRenderable<gui::SolidBgColor>(guiBatch, Color{ 32, 32, 32, 255 });
		auto text = frame->emplaceRenderable<gui::GuiText>(guiBatch, glyphs, engFont);
		text->textSettings().alignment.x = 0.0f;
		text->textSettings().alignment.y = 0.0f;
		text->textSettings().maxLineWidth = 200;
		text->textSettings().fontSize = 12;
		text->setText("Detected Graphics Card:\n    "+gfxCardInfo);
		frame->setVis(false);

		auto fac = gui::Factory{ guiBatch, glyphs, engFont };
		fac.makeSimpleButton(
			sys,
			gui::Rect{ 800-20, 0, 20, 20 },
			"V",
			Color{ 0, 0, 0, 255 },
			Color{ 128, 128, 128, 255 },
			Color{ 196, 196, 32, 255 },
			[=]() {
				frame->setVis(!frame->isVis());
			}
		);
	}
	
	auto pic = sweeper::HiddenImg{ disjointBatch , {50, 50, 400, 500}, TRAP_PICS};
	
	auto fac = gui::Factory{ guiBatch, glyphs, engFont };

	auto sweeperFrame = sys.emplaceChild<gui::Frame>(gui::Rect{ 500, 160, 300, 300 });

	auto boardState = sweeper::BoardState<10, 10>{};
	boardState.init(16);
	auto board = sweeper::BoardButtons<10, 10>(fac, sweeperFrame, { 0, 40 });

	auto label = sweeperFrame->emplaceChild<gui::Frame>(gui::Rect{ 200, 0, 40, 24 });
	label->emplaceRenderable<gui::SolidBgColor>(guiBatch, Color{ 32, 32, 32, 255 });
	auto minesLeft = label->emplaceRenderable<gui::GuiText>(guiBatch, glyphs, engFont);
	minesLeft->textSettings().color = { 255,0,0,255 };
	minesLeft->setText(std::to_string(boardState.getMinesLeft()));

	board.setCallback([&](sweeper::Coord coord) {
		fmt::print("{},{},{},\n", coord.x, coord.y, boardState.getProgress());
		if (cursor.button == cursor.LEFT) {
			boardState.explore(coord, true);
		}
		else if (cursor.button == cursor.RIGHT) {
			boardState.mark(coord);
		}
		
		board.refresh(boardState);
		minesLeft->setText(std::to_string(boardState.getMinesLeft()));
		pic.setProgress(boardState.getProgress() == 1.0f ? 1.0f : 0.05f + boardState.getProgress() * 0.55f);
	});

	fac.makeSimpleButton(
		sweeperFrame,
		gui::Rect{ 40, 0, 120, 24 },
		"New Game",
		Color{ 0, 0, 0, 255 },
		Color{ 128, 128, 128, 255 },
		Color{ 196, 196, 32, 255 },
		[&]() {
			if (boardState.getProgress() >= 1.0f) {
				pic.nextPic();
			}
			else {
				pic.setProgress(0.0f);
			}
			boardState.init(16);
			board.refresh(boardState);
			minesLeft->setText(std::to_string(boardState.getMinesLeft()));
		}
	);

	kb.onDown([&](auto& kb) {
#ifdef DEBUG
		pic.nextPic();
		pic.setProgress(1.0f);
#endif // DEBUG
	});
	cursor.onDown([&](const env::Cursor& c) {
		sys.cursorDown();
	});

	cursor.onUp([&](const env::Cursor& c) {
		sys.cursorUp();
	});

	cursor.onMove([&](const env::Cursor& c) {
		sys.cursorMove({ (i32)c.x, (i32)c.y });
	});

	auto fpsCounter = FPSCounter{};

	loop.flexStep([&](auto dt, auto ts) {		
		render::upload(batch);

		glClearColor(0.3, 0.3, 0.3, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		if (true) {
			auto trigs = 0;
			auto models = 0;
			trigs += render::drawWithoutUpload(batch, camera);
			models += batch.count();

			fpsCounter.tick(dt);

			render::upload(textBatch);
			render::drawWithoutUpload(textBatch, camera);

			glDisable(GL_CULL_FACE);

			render::upload(guiBatch);
			render::drawWithoutUpload(guiBatch, guiCamera);
		}
		
		
		if (true){
			glDisable(GL_CULL_FACE);
			pic.draw(guiCamera);
		}

		window.swapBuffer();

	});
	loop.start();
	return 0;
}

