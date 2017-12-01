#pragma once
#include "Transform.h"
#include "Components/Component.h"
#include "Editor/TypeRegister.h"
#include "Misc/Console.h"

#include <memory>

namespace Tristeon
{
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
		public:
			/**
			 * \brief Creates a new gameobject and initializes transform
			 */
			GameObject();

			/**
			 * \brief Creates a new gameobject and initializes transform
			 * \param registerObj determines if the gameobject should get registered to the scene or not
			 */
			explicit GameObject(bool registerObj);

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
			ReadOnlyProperty(Transform*, transform);
			GetProperty(transform) { return _transform.get(); }

			/**
			 * \brief Adds a component to the component list of type T
			 * \tparam T The type of component to be added
			 * \return Returns the new component
			 */
			template <typename T> std::shared_ptr<T> addComponent();

			/**
			 * \brief Gets the first component of the given type T
			 * \tparam T The type of the component to be searched for
			 * \return Returns the component
			 */
			template <typename T> std::shared_ptr<T> getComponent();
			/**
			 * \brief Gets all the components of type T
			 * \tparam T The type of the components to be searched for
			 * \return Returns a list of components of the given type T
			 */
			template <typename T> std::vector<std::shared_ptr<T>> getComponents();

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
			std::shared_ptr<Transform> _transform;
			/**
			 * \brief The list of components owned by this gameobject
			 */
			std::vector<std::shared_ptr<Components::Component>> components;

			/**
			 * \brief The local active state of this GameObject.
			 */
			bool active = true;
			
			//Registers GameObject to the type register
			REGISTER_TYPE_H(GameObject)
		};

		template <typename T>
		std::shared_ptr<T> GameObject::addComponent()
		{
			//Confirm that type T is a component
			if (!std::is_base_of<Components::Component, T>())
			{
				Misc::Console::error("Type T is not of type Component in addComponent<T>!");
				return nullptr;
			}

			//Create a new component of type T
			std::shared_ptr<T> component = std::make_shared<T>();

			//Initialize component
			component->setup(this);

			//Add to our component list
			components.push_back(component);

			//Return result for user reference
			return component;
		}

		template <typename T>
		std::shared_ptr<T> GameObject::getComponent()
		{
			//Confirm that type T is a component
			if (!std::is_base_of<Components::Component, T>())
			{
				Misc::Console::error("Type T is not of type Component in getComponent<T>!");
				return nullptr;
			}

			//Search for a component that can be cast to T
			for (std::shared_ptr<Components::Component> c : components)
			{
				if (dynamic_cast<T>(c.get()) != nullptr)
					return c;
			}

			//Return null if we can't find anything
			return nullptr;
		}

		/**
		 * \brief Overrides getComponent for T = Transform
		 * \return Returns the transform component
		 */
		template <>
		inline std::shared_ptr<Transform> GameObject::getComponent()
		{
			return _transform;
		}

		template <typename T>
		std::vector<std::shared_ptr<T>> GameObject::getComponents()
		{
			//Confirm that type T is a component
			if (!std::is_base_of<Components::Component, T>())
			{
				Misc::Console::error("Type T is not of type Component in getComponents<T>!");
				return nullptr;
			}

			//A list of components
			std::vector<std::shared_ptr<Components::Component>> result;

			//Look through the component list and find every component that can cast to T
			for (std::shared_ptr<Components::Component> c : components)
			{
				if (dynamic_cast<T>(c.get()) != nullptr)
					result.push_back(c);
			}

			//Return the resulting list
			return result;
		}
	}
}
