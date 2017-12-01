#include "SceneManager.h"
#include "Core/Message.h"
#include "Core/ManagerProtocol.h"
#include "Scene.h"
#include "Core/Rendering/Components/MeshRenderer.h"

namespace Tristeon
{
	namespace Scenes
	{
		Scene* SceneManager::activeScene = nullptr;

		SceneManager::~SceneManager()
		{
			delete activeScene;
		}

		void SceneManager::init()
		{
			activeScene = new Scene();
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
			activeScene = scene;
		}

		void SceneManager::reset()
		{
			//TODO: SceneManager::reset()
		}
	}
}
