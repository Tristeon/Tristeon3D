#pragma once
#include "Component.h"
#include "Misc/vector.h"

namespace Tristeon
{
	namespace Core
	{
		class Message;

		namespace Components
		{
			class Component;

			/**
			 * \brief ComponentManager manages the scripting components and calls their functions
			 */
			class ComponentManager
			{
			public:
				ComponentManager();
			private:
				/**
				 * \brief Calls function func on every component
				 * \tparam func The function to be called
				 */
				template <void (Component::*func)()>
				void callFunction();

				/**
				 * \brief Callback function that receives a message and tries to register a component based on that
				 * \param msg The message
				 */
				void registerComponent(Message msg);
				void deregisterComponent(Message msg);

				/**
				 * \brief The components
				 */
				vector<Component*> components;
			};

			template <void(Component::*func)()>
			void ComponentManager::callFunction()
			{
				//Call function func on every component in our list
				for (Component* c : components)
					(c->*func)();
			}
		}
	}
}
