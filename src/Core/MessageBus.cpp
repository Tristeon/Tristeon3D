#include "MessageBus.h"
#include "UserPrefs.h"
#include "Message.h"

namespace Tristeon
{
	namespace Core
	{
		std::map<MessageType, Misc::Delegate<Message>> MessageBus::messageCallbacks;

		void MessageBus::sendMessage(Message message)
		{
			validateMessageType(message.type);
			messageCallbacks[message.type].invoke(message);
		}

		void MessageBus::subscribeToMessage(MessageType type, std::function<void(Message)> f)
		{
			validateMessageType(type);
			messageCallbacks[type] += f;
		}

		void MessageBus::validateMessageType(MessageType type)
		{
			//Check for unknown message types
			if (!messageCallbacks.count(type))
				messageCallbacks[type] = Misc::Delegate<Message>();
		}
	}
}
