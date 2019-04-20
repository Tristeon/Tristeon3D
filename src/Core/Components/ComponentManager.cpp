#include "ComponentManager.h"
#include "Component.h"

#include "Core/MessageBus.h"
#include "Misc/Console.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Components
		{
			ComponentManager::ComponentManager()
			{
				//Subscribe to message events regarding callbacks and (de)registering of components
				MessageBus::subscribeToMessage(MT_SCRIPTINGCOMPONENT_REGISTER, [&](Message message) { registerComponent(message); });
				MessageBus::subscribeToMessage(MT_SCRIPTINGCOMPONENT_DEREGISTER, [&](Message message) { deregisterComponent(message); });
				MessageBus::subscribeToMessage(MT_START, [&](Message)       { callFunction<&Component::start>();		});
				MessageBus::subscribeToMessage(MT_UPDATE, [&](Message)      { callFunction<&Component::update>();		});
				MessageBus::subscribeToMessage(MT_LATEUPDATE, [&](Message)	{ callFunction<&Component::lateUpdate>();	});
				MessageBus::subscribeToMessage(MT_FIXEDUPDATE, [&](Message) { callFunction<&Component::fixedUpdate>();	});
				MessageBus::subscribeToMessage(MT_ONGUI, [&](Message)   { callFunction<&Component::onGUI>();		});
				MessageBus::subscribeToMessage(MT_GAME_LOGIC_START, [&](Message) { inPlayMode = true; });
				MessageBus::subscribeToMessage(MT_GAME_LOGIC_STOP, [&](Message) { inPlayMode = false; });
			}

			void ComponentManager::registerComponent(Message msg)
			{
				Misc::Console::t_assert(msg.userData != nullptr, "Trying to register a null component!");
				Component* c = dynamic_cast<Component*>(msg.userData);
				Misc::Console::t_assert(c != nullptr, "Failed to cast userData to component!");
				componentsToAdd.push_back(c);

				if (inPlayMode)
					c->start();
			}

			void ComponentManager::deregisterComponent(Message msg)
			{
				Misc::Console::t_assert(msg.userData != nullptr, "Trying to register a null component!");
				Component* c = dynamic_cast<Component*>(msg.userData);
				Misc::Console::t_assert(c != nullptr, "Failed to cast userData to component!");
				if (components.size() != 0)
					components.remove(c);
				if (componentsToAdd.size() != 0)
					componentsToAdd.remove(c);
			}

			void ComponentManager::addRegisteredComponents()
			{
				for (size_t i = 0; i < componentsToAdd.size(); i++)
					components.push_back(componentsToAdd[i]);
				componentsToAdd.clear();
			}
		}
	}
}
