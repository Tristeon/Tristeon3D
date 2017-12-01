#pragma once
#include "Core/Managers/Manager.h"
#include "Component.h"

namespace Tristeon
{
	namespace Core
	{
		//Forward decl
		class Message;

		namespace Components
		{
			//Forward decl
			class Component;

			/**
			 * \brief ComponentManager manages the scripting components and calls their functions
			 */
			class ComponentManager : public Managers::Manager
			{
			public:
				/**
				 * \brief Sets up the function callbacks
				 */
				void init() override;

			protected:
				/**
				 * \brief Removes all references to registered components
				 */
				void reset() override;

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

				/**
				 * \brief The components
				 */
				std::vector<Component*> components;
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
