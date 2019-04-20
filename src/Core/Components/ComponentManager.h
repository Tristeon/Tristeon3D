#pragma once
#include "Component.h"
#include "Misc/vector.h"
#include <XPlatform/access.h>

TRISTEON_UNIQUE_ACCESS_DECL()
namespace Tristeon
{
	namespace Core
	{
		class Message;
		class Engine;

		namespace Components
		{
			class Component;

			/**
			 * ComponentManager keeps track of existing components and runs callbacks on them when needed.
			 * ComponentManager implements a basic (de)register system that listens to the initailization of new components.
			 *
			 * This class is not intended to be accessed or used by users.
			 */
			class ComponentManager final
			{
				TRISTEON_UNIQUE_ACCESS(ComponentManager)

				ComponentManager();
				/**
				 * Calls function f on every registered component
				 */
				template <void (Component::*func)()>
				void callFunction();

				/**
				 * Adds the component that is attached to Message to the component list
				 * \exception runtime_error If msg.userData is null or when msg.userData can not successfuly cast to Component
				 */
				void registerComponent(Message msg);
				/**
				 * Removes the component that is attached to Message from the component list
				 * \exception runtime_error If msg.userData is null or if msg.userData can not successfuly cast to Component
				 */
				void deregisterComponent(Message msg);

				void addRegisteredComponents();

				vector<Component*> components;
				vector<Component*> componentsToAdd;

				bool inPlayMode = false;
			};

			template <void(Component::*func)()>
			void ComponentManager::callFunction()
			{
				addRegisteredComponents(); //Components are sometimes created during "c->*func" so we register before and after the loop is done

				for (Component* c : components)
					(c->*func)();

				addRegisteredComponents();
			}
		}
	}
}
