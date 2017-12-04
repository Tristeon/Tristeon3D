#include "SceneManager.h"
#include "Core/Message.h"
#include "Core/ManagerProtocol.h"
#include "Scene.h"
#include "Core/Rendering/Components/MeshRenderer.h"

namespace Tristeon
{
	namespace Scenes
	{
		std::unique_ptr<Scene> SceneManager::activeScene = nullptr;

		SceneManager::~SceneManager()
		{
		}

		void SceneManager::init()
		{
			activeScene = std::make_unique<Scene>();
		}

		void SceneManager::loadScene(int id)
		{
		}

		void SceneManager::loadScene(std::string name)
		{
			Core::ManagerProtocol::sendMessage(Core::MT_MANAGER_RESET);
			//TODO: Load scene
		}

		void SceneManager::loadScene(Scene* scene)
		{
			activeScene = std::unique_ptr<Scene>(scene);
		}

		void SceneManager::reset()
		{
			//TODO: SceneManager::reset()
		}
	}
}
