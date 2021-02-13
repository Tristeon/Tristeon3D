#include "ComponentManager.h"
#include <Core/MessageBus.h>

namespace Tristeon::Core::Components
{
	ComponentManager::ComponentManager()
	{
		MessageBus::subscribe(Message::Type::Start, [&](Message) { callFunction<&Component::start>(); });
		MessageBus::subscribe(Message::Type::EarlyUpdate, [&](Message) { callFunction<&Component::earlyUpdate>(); });
		MessageBus::subscribe(Message::Type::Update, [&](Message) { callFunction<&Component::update>(); });
		MessageBus::subscribe(Message::Type::LateUpdate, [&](Message) { callFunction<&Component::lateUpdate>(); });
		MessageBus::subscribe(Message::Type::FixedUpdate, [&](Message) { callFunction<&Component::fixedUpdate>(); });
	}
}