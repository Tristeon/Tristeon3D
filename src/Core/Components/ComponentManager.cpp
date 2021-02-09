#include "ComponentManager.h"
#include <Core/MessageBus.h>

namespace Tristeon::Core::Components
{
	ComponentManager::ComponentManager()
	{
		MessageBus::subscribeToMessage(MT_START, [&](Message) { callFunction<&Component::start>(); });
		MessageBus::subscribeToMessage(MT_UPDATE, [&](Message) { callFunction<&Component::update>(); });
		MessageBus::subscribeToMessage(MT_LATEUPDATE, [&](Message) { callFunction<&Component::lateUpdate>(); });
		MessageBus::subscribeToMessage(MT_FIXEDUPDATE, [&](Message) { callFunction<&Component::fixedUpdate>(); });
	}
}