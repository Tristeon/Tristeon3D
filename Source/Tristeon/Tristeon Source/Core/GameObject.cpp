#include "GameObject.h"
#include "Scenes/SceneManager.h"

namespace Tristeon
{
	namespace Core
	{
		//Registers GameObject to the type register
		REGISTER_TYPE_CPP(GameObject)

		GameObject::GameObject()
		{
			//Create transform
			_transform = std::make_shared<Transform>();
			//Increment instance count of the current scene
			Scenes::SceneManager::getActiveScene()->instanceCount++;
		}

		GameObject::GameObject(bool reg)
		{
			//Create transform
			_transform = std::make_shared<Transform>();

			if (reg)
			{
				//Increment instance count of the current scene
				Scenes::SceneManager::getActiveScene()->instanceCount++;
			}
		}

		void GameObject::init()
		{
			//Init all our components
			for (unsigned int i = 0; i < components.size(); i++)
				components[i]->init();
		}

		nlohmann::json GameObject::serialize()
		{
			//Write gameobject data into json
			nlohmann::json output;
			output["typeID"] = typeid(GameObject).name();
			output["instanceID"] = getInstanceID();
			output["active"] = active;
			output["name"] = name;
			output["tag"] = tag;
			output["transform"] = _transform->serialize();
			nlohmann::json serializedComponents = nlohmann::json::array_t();
			for (auto component : components)
				serializedComponents.push_back(component->serialize());
			output["components"] = serializedComponents;
			return output;
		}

		void GameObject::deserialize(nlohmann::json json)
		{
			//Read gameobject data from json
			if (json["active"].is_boolean()) active = json["active"];
			const std::string nameValue = json["name"];
			name = nameValue;
			const std::string tagValue = json["tag"];
			tag = tagValue;
			_transform->deserialize(json["transform"]);
			components.clear();
			for (auto serializedComponent : json["components"])
			{
				//TODO: instead of recreating identify already existing components instead of removing those and load those
				//in to avoid weird behavior

				//Create an instance using the given typeid, this creates an instance of the type
				//which was serialized using its unique ID thus to retrieve the type.
				auto* serializable = TypeRegister::createInstance(serializedComponent["typeID"]);
				//Extra steps for clarity
				Components::Component* component = (Components::Component*) serializable;
				component->init();
				component->setup(this);
				std::shared_ptr<Components::Component> sharedComponent(component);
				sharedComponent->deserialize(serializedComponent);
				components.push_back(sharedComponent);
			}
		}
	}
}
