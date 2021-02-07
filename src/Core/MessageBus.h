#pragma once
#include <map>
#include "Message.h"
#include "Misc/Delegate.h"

namespace Tristeon::Core
{
	/**
	 * The MessageBus is an abstract message bus that allows for subsystems to communicate information back/forth
	 * without knowledge of the existence one another.
	 */
	class MessageBus final
	{
	public:
		/**
		 * Sends a message to all listeners subscribed to message.type
		 */
		static void sendMessage(Message message);

		/**
		 * Adds a function to the message callbacks based on the given message type
		 * \param type The type of message the given function should listen to
		 */
		static void subscribeToMessage(MessageType type, std::function<void(Message)> f);

	private:
		/**
		 * Validates if the given messagetype has already been added to the callback system. If not, the type will be added.
		 */
		static void validateMessageType(MessageType type);

		static std::map<MessageType, Misc::Delegate<Message>> messageCallbacks;
	};
}