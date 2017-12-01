#include "Application.h"

namespace Tristeon
{
	Application::~Application()
	{
		//Cleanup
#ifdef EDITOR
		delete editor;
#endif
		delete engine;
	}

	void Application::init()
	{
		//Init engine
		engine = new Core::Engine();
		engine->init();

		//Init editor
#ifdef EDITOR
		editor = new Editor::TristeonEditor();
		editor->init(engine);
#endif
		engine->run();

		//Auto start game if we are running in release mode
#ifndef EDITOR
		engine->startGame();
#endif
	}
}
