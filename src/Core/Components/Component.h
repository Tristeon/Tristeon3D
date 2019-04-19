﻿#pragma once
#include "Core/TObject.h"
#include "Misc/Property.h"

namespace Tristeon
{
	namespace Core
	{
		class GameObject;
		class MessageBus;
		class Transform;

		namespace Components
		{
			/**
			 * Component is the base class of all components in the engine. A component is a piece of behavior that can be added to a gameobject.
			 */
			class Component : public TObject
			{
				friend GameObject;

			public:
				/**
				 * Deregisters itself from engine callbacks
				 */
				~Component();

				/**
				* The gameobject that this component is attached to
				*/
				ReadOnlyProperty(Component, gameObject, GameObject*);
				GetProperty(gameObject) { return _gameObject; }

				/**
				* The transform of the gameobject that this component is attached to
				*/
				ReadOnlyProperty(Component, transform, Core::Transform*);
				GetProperty(transform);

				/**
				 * Initializes the component and registers itself to engine callbacks. Can be overriden
				 */
				virtual void init();

				/**
				 * Start gets called when the scene is first run
				 */
				virtual void start() {}
				/**
				 * Update gets called every frame when the game is running
				 */
				virtual void update() {}
				/**
				 * FixedUpdate gets called a fixed amount of times a second
				 */
				virtual void fixedUpdate() {}
				/**
				 * LateUpdate gets called after all the other update calls
				 */
				virtual void lateUpdate() {}

				/**
				 * OnGUI gets called before anything is rendered. Use this to draw debug visuals using debugDrawManager.
				 * OnGUI gets called both in playmode and in editor mode.
				 */
				virtual void onGUI() {}

				/**
				 * Gets the first component of the given type T. Null if no matching component can be found.
				 */
				template <typename T> T* getComponent() { return gameObject.get()->getComponent<T>(); }

				/**
				 * Gets all the components of type T. Will return an empty vector if no components can be found.
				 */
				template <typename T> std::vector<T*> getComponents() { return gameObject.get()->getComponents<T>(); }
			private:
				/**
				 * Stores the gameobject it's attached to. Only GameObject can call this function
				 * \param go
				 */
				void setup(GameObject* go);
				/**
				 * The gameobject this component is attached to
				 */
				GameObject* _gameObject = nullptr;
			protected:
				bool registered = false;
			};
		}
	}
}
