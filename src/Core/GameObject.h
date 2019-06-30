#pragma once
#include "Transform.h"
#include "Components/Component.h"
#include "Editor/TypeRegister.h"
#include "Misc/Console.h"
#include <memory>
#include "Physics/Tests/OBB.h"

namespace Tristeon
{
#ifdef TRISTEON_EDITOR
	namespace Editor { class EditorNodeTree; class EditorNode; }
#endif
	namespace Scenes { class SceneManager; class Scene; }

	namespace Core
	{
		class MessageBus;

		/**
		 * GameObject is a container of components, with an instance of Transform which describes its position, scale and rotation.
		 * GameObject is the only type of entity that is directly allowed as an instance in the scene.
		 * Its behavior is defined by the attached components (e.g. it might be visible by adding a renderer, might move around through a movement behavior etc)
		 */
		class GameObject : public TObject
		{
			friend Scenes::Scene;
			friend Scenes::SceneManager;
#ifdef TRISTEON_EDITOR
			friend Editor::EditorNode;
			friend Editor::EditorNodeTree;
#endif
		public:
			GameObject();
			std::string tag;

			/**
			* The transform of the gameobject. Describes position, rotation, scale and a parent-child structure.
			*/
			ReadOnlyProperty(GameObject, transform, Transform*);
			GetProperty(transform) { return _transform.get(); }

			/**
			 * Creates a new component of type T and adds it to the component list. Will also return the newly created component.
			 */
			template <typename T>
			typename std::enable_if<std::is_base_of<Components::Component, T>::value, T>::type* addComponent();

			/**
			 * Gets the first component of the given type T. Null if no matching component can be found.
			 */
			template <typename T> T* getComponent();

			/**
			 * Gets all the components of type T. Will return an empty vector if no components can be found.
			 */
			template <typename T> std::vector<T*> getComponents();

			static GameObject* find(const std::string& name);

			nlohmann::json serialize() override;
			void deserialize(nlohmann::json json) override;

			bool isActive() const { return active; }
		private:
			/**
			 * Initializes all the gameobjects' components. 
			 * This function gets called after the scene has been fully loaded. The game does not have to be running.
			 */
			void init();

			std::unique_ptr<Transform> _transform;
			std::vector<std::unique_ptr<Components::Component>> components;

			bool active = true;

			/**
			 * The filepath of the prefab this GameObject might be attached to. "" if it's not a prefab.
			 */
			std::string prefabFilePath = "";

			REGISTER_TYPE_H(GameObject)
		};


		template <typename T>
        typename std::enable_if<std::is_base_of<Components::Component, T>::value, T>::type* GameObject::addComponent()
		{
			T* component = new T();
			component->setup(this);
			components.push_back(std::move(std::unique_ptr<T>(component)));
			return component;
		}

		template <typename T>
		auto GameObject::getComponent() -> T*
		{
			if (!std::is_base_of<Components::Component, T>())
			{
				Misc::Console::error("Type T is not of type Component in getComponent<T>!");
				return nullptr;
			}
			for (int i = 0; i < components.size(); i++)
			{
				T* component = dynamic_cast<T*>(components[i].get());
				if (component != nullptr)
					return component;
			}
			return nullptr;
		}

		/**
		 * Overrides getComponent for T = Transform
		 * \return Returns the transform component
		 */
		template <>
		inline Transform* GameObject::getComponent()
		{
			return _transform.get();
		}

		template <typename T>
		std::vector<T*> GameObject::getComponents()
		{
			if (!std::is_base_of<Components::Component, T>())
			{
				Misc::Console::error("Type T is not of type Component in getComponents<T>!");
				return nullptr;
			}

			std::vector<Components::Component*> result;
			for (int i = 0; i < components.size(); i++)
			{
				if (dynamic_cast<T*>(components[i].get()) != nullptr)
					result.push_back(dynamic_cast<T*>(components[i].get()));
			}
			return result;
		}
	}
}
