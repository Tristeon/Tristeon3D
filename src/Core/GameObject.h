#pragma once
#include "Transform.h"
#include "Components/Component.h"
#include "Editor/TypeRegister.h"
#include "Misc/Console.h"
#include <memory>

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
		 * \brief GameObject is a container of gameobjects. It also owns an instance of Transform, and it is the only type of object that is directly allowed in the scene.
		 */
		class GameObject : public TObject
		{
			//Scenes can access our information
			friend Scenes::Scene;
			friend Scenes::SceneManager;
#ifdef TRISTEON_EDITOR
			friend Editor::EditorNode;
			friend Editor::EditorNodeTree;
#endif
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
			using ComponentIsBase = std::enable_if<std::is_base_of<Components::Component, T>::value>;

			/**
			 * \brief Adds a component to the component list of type T
			 * \tparam T The type of component to be added
			 * \return Returns the new component
			 */
			template <typename T>
			typename std::enable_if<std::is_base_of<Components::Component, T>::value, T>::type* addComponent();

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


		template <typename T>
        typename std::enable_if<std::is_base_of<Components::Component, T>::value, T>::type* GameObject::addComponent()
		{
			T* component = new T();
			component->setup(this);
			components.push_back(std::move(std::unique_ptr<T>(component)));
			return component;
		}

		template <typename T>
		T* GameObject::getComponent()
		{
			if (!std::is_base_of<Components::Component, T>())
			{
				Misc::Console::error("Type T is not of type Component in getComponent<T>!");
				return nullptr;
			}

			for (int i = 0; i < components.size(); i++)
			{
				if (dynamic_cast<T>(components[i].get()) != nullptr)
					return components[i].get();
			}
			return nullptr;
		}

		/**
		 * \brief Overrides getComponent for T = Transform
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
				if (dynamic_cast<T>(components[i].get()) != nullptr)
					result.push_back(components[i].get());
			}
			return result;
		}
	}
}
