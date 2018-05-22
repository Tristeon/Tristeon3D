#pragma once
#include "Transform.h"
#include "Components/Component.h"
#include "Editor/TypeRegister.h"
#include "Misc/Console.h"

#include <memory>
#include "XPlatform/enable_template.h"

namespace Tristeon
{
	namespace Editor {
		class EditorNodeTree;
		class EditorNode;
	}

	//Forward decl
	namespace Scenes
	{
		class SceneManager;
		class Scene;
	}

	namespace Core
	{
		//Forward decl
		class ManagerProtocol;

		/**
		 * \brief GameObject is a container of gameobjects. It also owns an instance of Transform, and it is the only type of object that is directly allowed in the scene.
		 */
		class GameObject : public TObject
		{
			//Scenes can access our information
			friend Scenes::Scene;
			friend Scenes::SceneManager;
			friend Editor::EditorNode;
			friend Editor::EditorNodeTree;
		public:
			/**
			 * \brief Creates a new gameobject and initializes transform
			 */
			GameObject();

			/**
			 * \brief Initializes all the gameobjects' components. This function gets called after the scene has been fully loaded. The game does not have to be running
			 */
			void init();

			/**
			 * \brief The tag of this gameobject
			 */
			std::string tag;

			/**
			* \brief The transform of the gameobject. Describes position, rotation, scale and a parent-child structure
			*/
			ReadOnlyProperty(GameObject, transform, Transform*);
			GetProperty(transform) { return _transform.get(); }

			template <typename T>
			using ComponentIsBase = std::enable_if_t<std::is_base_of<Components::Component, T>::value, T>;

			/**
			 * \brief Adds a component to the component list of type T
			 * \tparam T The type of component to be added
			 * \return Returns the new component
			 */
			template <typename T, TRISTEON_ENABLE_TEMPLATE ComponentIsBase<T>> T* addComponent();

			/**
			 * \brief Gets the first component of the given type T
			 * \tparam T The type of the component to be searched for
			 * \return Returns the component
			 */
			template <typename T> T* getComponent();
			/**
			 * \brief Gets all the components of type T
			 * \tparam T The type of the components to be searched for
			 * \return Returns a list of components of the given type T
			 */
			template <typename T> std::vector<T*> getComponents();

			/**
			 * \brief Serializes the gameobject into a json structure
			 * \return Returns a json structure containing the gameobject data
			 */
			nlohmann::json serialize() override;
			/**
			 * \brief Deserializes the gameobject from a json structure
			 * \param json The json structure containing the gameobject data
			 */
			void deserialize(nlohmann::json json) override;
		private:
			/**
			 * \brief The transform. Describes position, rotation, scale and the parent-child structure. GameObject creates and destroys this object
			 */
			std::unique_ptr<Transform> _transform;
			/**
			 * \brief The list of components owned by this gameobject
			 */
			std::vector<std::unique_ptr<Components::Component>> components;

			/**
			 * \brief The local active state of this GameObject.
			 */
			bool active = true;

			std::string prefabFilePath = "";

			//Registers GameObject to the type register
			REGISTER_TYPE_H(GameObject)
		};

		template <typename T, TRISTEON_ENABLE_TEMPLATE std::enable_if_t<std::is_base_of<Components::Component, T>::value, T>>
		T* GameObject::addComponent()
		{
			//Create a new component of type T
			T* component = new T();

			//Initialize component
			component->setup(this);

			//Add to our component list
			components.push_back(std::move(std::unique_ptr<T>(component)));

			//Return result for user reference
			return component;
		}

		template <typename T>
		T* GameObject::getComponent()
		{
			//Confirm that type T is a component
			if (!std::is_base_of<Components::Component, T>())
			{
				Misc::Console::error("Type T is not of type Component in getComponent<T>!");
				return nullptr;
			}

			//Search for a component that can be cast to T
			for (int i = 0; i < components.size(); i++)
			{
				if (dynamic_cast<T>(components[i].get()) != nullptr)
					return components[i].get();
			}

			//Return null if we can't find anything
			return nullptr;
		}

		/**
		 * \brief Overrides getComponent for T = Transform
		 * \return Returns the transform component
		 */
		template <>
		inline Core::Transform* Core::GameObject::getComponent()
		{
			return _transform.get();
		}

		template <typename T>
		std::vector<T*> GameObject::getComponents()
		{
			//Confirm that type T is a component
			if (!std::is_base_of<Components::Component, T>())
			{
				Misc::Console::error("Type T is not of type Component in getComponents<T>!");
				return nullptr;
			}

			//A list of components
			std::vector<Components::Component*> result;

			//Look through the component list and find every component that can cast to T
			for (int i = 0; i < components.size(); i++)
			{
				if (dynamic_cast<T>(components[i].get()) != nullptr)
					result.push_back(components[i].get());
			}

			//Return the resulting list
			return result;
		}
	}
}
