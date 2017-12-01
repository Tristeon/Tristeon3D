#pragma once
#include "Core/TObject.h"
#include "Misc/Property.h"

namespace Tristeon
{
	namespace Core
	{
		//Forward decl
		class GameObject;
		class ManagerProtocol;
		class Transform;

		namespace Components
		{
			/**
			 * \brief Component is the base class of all components in the engine. A component can be added to a gameobject.
			 */
			class Component : public TObject
			{
				friend GameObject;

			public:
				/** 
				 * \brief Deregisters itself from engine callbacks
				 */
				~Component();

				/**
				* \brief The gameobject that this component is attached to
				*/
				ReadOnlyProperty(Core::GameObject*, gameObject);
				GetPropertyConst(gameObject) { return _gameObject; }

				/**
				* \brief The transform of the gameobject that this component is attached to
				*/
				ReadOnlyProperty(Core::Transform*, transform);
				Transform* property__get_transform() const;

				/**
				 * \brief Initializes the component and registers itself to engine callbacks. Can be overriden
				 */
				virtual void init();

				/**
				 * \brief Start gets called when the scene is first run
				 */
				virtual void start() {}
				/**
				 * \brief Update gets called every frame when the game is running
				 */
				virtual void update() {}
				/**
				 * \brief FixedUpdate gets called a fixed amount of times a second
				 */
				virtual void fixedUpdate() {}
				/**
				 * \brief LateUpdate gets called after all the other update calls
				 */
				virtual void lateUpdate() {}
			private:
				/**
				 * \brief Stores the gameobject it's attached to. Only GameObject can call this function
				 * \param go 
				 */
				void setup(GameObject* go);
				/**
				 * \brief The gameobject this component is attached to
				 */
				GameObject* _gameObject = nullptr;
			protected:
				bool registered = false;
			};
		}
	}
}