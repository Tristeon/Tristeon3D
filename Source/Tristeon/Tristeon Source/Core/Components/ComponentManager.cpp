#include "ComponentManager.h"
#include "Component.h"

#include "Core/ManagerProtocol.h"
#include "Misc/Console.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Components
		{
			void ComponentManager::init()
			{
				//Registers the reset function
				Manager::init();

				//Register component
				ManagerProtocol::subscribeToMessage(MT_SCRIPTINGCOMPONENT_REGISTER, [&](Message message) { registerComponent(message); });
				ManagerProtocol::subscribeToMessage(MT_SCRIPTINGCOMPONENT_DEREGISTER, [&](Message message) { deregisterComponent(message); });

				//Gameplay functions
				ManagerProtocol::subscribeToMessage(MT_START, [&](Message)       { callFunction<&Component::start>(); });
				ManagerProtocol::subscribeToMessage(MT_UPDATE, [&](Message)      { callFunction<&Component::update>(); });
				ManagerProtocol::subscribeToMessage(MT_LATEUPDATE, [&](Message)  { callFunction<&Component::lateUpdate>(); });
				ManagerProtocol::subscribeToMessage(MT_FIXEDUPDATE, [&](Message) { callFunction<&Component::fixedUpdate>(); });
			}

			void ComponentManager::registerComponent(Message msg)
			{
				//Confirm that msg.userdata is set, to prevent unneeded casting
				Misc::Console::t_assert(msg.userData != nullptr, "Trying to register a null component!");
				
				//Cast
				Component* c = dynamic_cast<Component*>(msg.userData);
				
				//Comfirm that the cast succeeded
				Misc::Console::t_assert(c != nullptr, "Failed to cast userData to component!");
				
				//Add to our componentlist
				components.push_back(c);
			}

			void ComponentManager::deregisterComponent(Message msg)
			{
				//Confirm that msg.userdata is set, to prevent unneeded casting
				Misc::Console::t_assert(msg.userData != nullptr, "Trying to register a null component!");

				//Cast
				Component* c = dynamic_cast<Component*>(msg.userData);

				//Comfirm that the cast succeeded
				Misc::Console::t_assert(c != nullptr, "Failed to cast userData to component!");

				//Remove from components
				if (components.size() != 0)
					components.remove(c);
			}

			void ComponentManager::reset()
			{
				//Clears the component list
				components.clear();
			}
		}
	}
}
