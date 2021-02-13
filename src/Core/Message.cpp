#include "Message.h"

namespace Tristeon::Core
{
	Message::Message(Type type)
	{
		this->type = type;
		this->userData = nullptr;
	}

	Message::Message(Type type, TObject* userData)
	{
		this->type = type;
		this->userData = userData;
	}
}