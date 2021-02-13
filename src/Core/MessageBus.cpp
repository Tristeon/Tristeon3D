#include "MessageBus.h"
#include "Message.h"

namespace Tristeon::Core
{
	std::map<Message::Type, Misc::Delegate<Message>> MessageBus::messageCallbacks;

	void MessageBus::send(Message message)
	{
		validate(message.type);
		messageCallbacks[message.type].invoke(message);
	}

	void MessageBus::subscribe(Message::Type type, std::function<void(Message)> f)
	{
		validate(type);
		messageCallbacks[type] += f;
	}

	void MessageBus::validate(Message::Type type)
	{
		if (!messageCallbacks.count(type))
			messageCallbacks[type] = Misc::Delegate<Message>();
	}
}