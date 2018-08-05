#pragma once
#include <map>
#include "Message.h"
#include "Misc/Delegate.h"

namespace Tristeon
{
	namespace Core
	{
		/**
		 * The MessageBus is an abstract message bus that allows for subsystems to communicate information back/forth
		 * without knowledge of the existence one another.
		 */
		class MessageBus final
		{
		public:
			/**
			 * \brief Sends a message to every subsystem that has subscribed to message.type
			 */
			static void sendMessage(Message message);

			/**
			 * \brief Adds a function to the message callbacks based on the given message type
			 * \param type The type of message the given function should listen to
			 * \param f The callback function
			 */
			static void subscribeToMessage(MessageType type, std::function<void(Message)> f);

		private:
			/**
			 * \brief Validates if the given messagetype has already been added to the callback system. If not, the type will be added
			 */
			static void validateMessageType(MessageType type);

			/**
			 * \brief The message callback list, stored as a map/dictionary for easy access.
			 * Delegate is a custom class, wrapping around std::function
			 */
			static std::map<MessageType, Misc::Delegate<Message>> messageCallbacks;
		};
	}
}
