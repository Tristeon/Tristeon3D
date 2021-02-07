#include "Message.h"

namespace Tristeon::Core
{
	Message::Message(MessageType type)
	{
		this->type = type;
		this->userData = nullptr;
	}

	Message::Message(MessageType type, TObject* userData)
	{
		this->type = type;
		this->userData = userData;
	}
}