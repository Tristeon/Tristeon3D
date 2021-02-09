#include "SceneManager.h"
#include "Scene.h"
#include "Core/Message.h"
#include "Core/MessageBus.h"
#include "Core/Rendering/Components/MeshRenderer.h"
#include "Core/JsonSerializer.h"

namespace Tristeon::Core
{
	std::unique_ptr<Scene> SceneManager::_current = nullptr;
	std::map<std::string, std::string> SceneManager::_paths;

	SceneManager::SceneManager()
	{
		_current = std::make_unique<Scene>();
		_current->name = "UnNamed";

		//Load scenes into the manager
		std::ifstream stream("Scenes.ProjectSettings", std::fstream::in | std::fstream::out | std::fstream::app);
		nlohmann::json json;
		stream >> json;

		//Invalid json error handling
		if (json.is_null())
		{
			std::cout << "file is either a non-json file or corrupted" << std::endl;
			throw std::invalid_argument("file is either a non-json file or corrupted");
		}
		for (auto iterator = json.begin(); iterator != json.end(); ++iterator)
		{
			const std::string stringValue = iterator.value();
			_paths[iterator.key()] = stringValue;
		}
	}

	void SceneManager::load(int id)
	{
		load(_paths.begin()->first);
	}

	void SceneManager::load(std::string name)
	{
		MessageBus::sendMessage(MT_MANAGER_RESET);

		//Attempt to deserialize scene from file
		auto const scene = JsonSerializer::deserialize<Scene>(_paths[name]);
		if (!scene)
		{
			Misc::Console::warning("Couldn't load scene " + std::string(_paths[name]));
			_current = std::unique_ptr<Scene>();
			return;
		}
		load(scene);
	}

	void SceneManager::load(Scene* scene)
	{
		MessageBus::sendMessage(MT_MANAGER_RESET);
		_current = std::unique_ptr<Scene>(scene);
		createParentalBonds(_current.get());
	}

	Transform* SceneManager::findTransformByInstanceID(std::string instanceID)
	{
		for (int i = 0; i < _current->gameObjects.size(); ++i)
		{
			if (_current->gameObjects[i]->transform.get()->getInstanceID() == instanceID)
				return _current->gameObjects[i].get()->transform.get();
		}
		return nullptr;
	}

	void SceneManager::createParentalBonds(Scene* scene)
	{
		std::vector<std::unique_ptr<GameObject>>& gameObjects = scene->gameObjects;
		for (int i = 0; i < gameObjects.size(); ++i)
		{
			auto parent = gameObjects[i]->transform.get()->parentID;
			//Does gameobject have a parent?
			if (parent != "null")
			{
				//Find and set the parent
				gameObjects[i]->transform.get()->setParent(findTransformByInstanceID(parent));
			}
		}
	}

	void SceneManager::addScenePath(std::string name, std::string path)
	{
		_paths[name] = path;
		nlohmann::json json(_paths);
		JsonSerializer::serialize("Scenes.ProjectSettings", json);
	}
}