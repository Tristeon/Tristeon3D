#include "Scene.h"

namespace Tristeon
{
	namespace Scenes
	{
		REGISTER_TYPE_CPP(Scene)

		Scene::Scene()
		{
		}

		Scene::~Scene()
		{
			//Force reset just to be sure
			for(auto go : gameObjects)
			{
				go.reset();
			}
			gameObjects.clear();
		}

		void Scene::init()
		{
			for (int i = 0; i < gameObjects.size(); i++)
				gameObjects[i]->init();
		}

		nlohmann::json Scene::serialize()
		{
			nlohmann::json output;
			output["typeID"] = typeid(Scene).name();
			output["name"] = name;
			nlohmann::json jsonArray = nlohmann::json::array();
			for (auto gameObject : gameObjects)
			{
				jsonArray.push_back(gameObject->serialize());
			}
			output["gameObjects"] = jsonArray;
			return output;
		}

		void Scene::deserialize(nlohmann::json json)
		{
			nlohmann::json gameObjectData = json["gameObjects"];
			if (gameObjectData.is_array())
			{
				for (auto iterator = gameObjectData.begin(); iterator != gameObjectData.end(); ++iterator)
				{
					std::shared_ptr<Core::GameObject> gameObject = std::make_shared<Core::GameObject>();
					gameObject->deserialize(iterator->get<nlohmann::json>());
					gameObjects.push_back(gameObject);
				}
			} else
			{
				std::cout << "Deserialization of the scene is going goofy, ur probably deserializing the scene with a wrong json format";
			}
			const std::string nameValue = json["name"];
			name = nameValue;
		}
	}
}
