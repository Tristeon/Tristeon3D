#include "Application.h"

#ifdef TRISTEON_EDITOR
#include "Scenes/SceneManager.h"
#else
#include "Core/MessageBus.h"
#endif

namespace Tristeon
{
	Application::Application()
	{
		engine = std::make_unique<Core::Engine>();

#ifdef TRISTEON_EDITOR
		editor = std::make_unique<Editor::TristeonEditor>(engine.get());
#else
		//Auto start game if there's no editor
		Scenes::SceneManager::loadScene(0);
		Core::MessageBus::sendMessage(Core::MessageType::MT_GAME_LOGIC_START);
#endif
		engine->run();
	}
}