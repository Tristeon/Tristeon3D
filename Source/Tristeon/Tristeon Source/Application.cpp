#include "Application.h"

namespace Tristeon
{
	Application::~Application()
	{
		//Cleanup
#ifdef EDITOR
		editor.reset();
#endif
		engine.reset();
	}

	void Application::init()
	{
		//Init engine
		engine = std::make_unique<Core::Engine>();
		engine->init();

		//Init editor
#ifdef EDITOR
		editor = std::make_unique<Editor::TristeonEditor>();
		editor->init(engine.get());
#endif
		engine->run();

		//Auto start game if we are running in release mode
#ifndef EDITOR
		engine->startGame();
#endif
	}
}
