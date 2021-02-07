#include "GameObject.h"
#include "Scenes/SceneManager.h"

namespace Tristeon
{
	namespace Core
	{
		REGISTER_TYPE_CPP(GameObject)

		GameObject::GameObject()
		{
			_transform = std::make_unique<Transform>();
		}

		void GameObject::init()
		{
			for (unsigned int i = 0; i < components.size(); i++)
				components[i]->init();
		}

		nlohmann::json GameObject::serialize()
		{
			nlohmann::json output;
			output["typeID"] = Core::Type<GameObject>::fullName();
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

		//Temporary till the std::string from nlohmann::json is fixed
		#define GET_STRING(name, stringName) const std::string value_##name = json[stringName]; name = value_##name;

		void GameObject::deserialize(nlohmann::json json)
		{
			GET_STRING(instanceID, "instanceID");
			active = json["active"];
			GET_STRING(name, "name");
			GET_STRING(tag, "tag");
			if (!json["prefabFilePath"].is_null())
			{
				GET_STRING(prefabFilePath, "prefabFilePath");
			}
			_transform->deserialize(json["transform"]);
			components.clear();
			for (auto serializedComponent : json["components"])
			{
				//TODO: instead of recreating identify already existing components instead of removing those and load those
				//in to avoid weird behavior and increase performance

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