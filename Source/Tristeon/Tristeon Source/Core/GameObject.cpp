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
			_transform = std::make_unique<Transform>();
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
			for (int i = 0; i < components.size(); ++i)
				serializedComponents.push_back(components[i]->serialize());

			output["components"] = serializedComponents;
			output["prefabFilePath"] = prefabFilePath;
			return output;
		}

		void GameObject::deserialize(nlohmann::json json)
		{
			//Read gameobject data from json
			const std::string instanceIDValue = json["instanceID"];
			instanceID = instanceIDValue;
			active = json["active"];
			const std::string nameValue = json["name"];
			name = nameValue;
			const std::string tagValue = json["tag"];
			tag = tagValue;
			const std::string prefabFilePathValue = json["prefabFilePath"];
			prefabFilePath = prefabFilePathValue;
			_transform->deserialize(json["transform"]);
			components.clear();
			for (auto serializedComponent : json["components"])
			{
				//TODO: instead of recreating identify already existing components instead of removing those and load those
				//in to avoid weird behavior

				//Create an instance using the given typeid, this creates an instance of the type
				//which was serialized using its unique ID thus to retrieve the type.
				auto serializable = TypeRegister::createInstance(serializedComponent["typeID"]);
				Components::Component* component = (Components::Component*) serializable.get();
				component->init();
				component->setup(this);
				serializable.release();
				std::unique_ptr<Components::Component> sharedComponent(component);
				sharedComponent->deserialize(serializedComponent);
				components.push_back(std::move(sharedComponent));
			}
		}
	}
}
