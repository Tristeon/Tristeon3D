#include "Component.h"
#include "Core/MessageBus.h"
#include "Core/GameObject.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Components
		{
			Component::~Component()
			{
				if (registered)
					MessageBus::sendMessage({ MT_SCRIPTINGCOMPONENT_DEREGISTER, this });
			}

			Component::property__tmp_type_transform Component::get_transform()
			{
				return _gameObject->transform.get();
			}

			void Component::init()
			{
				if (!registered)
					MessageBus::sendMessage({ MT_SCRIPTINGCOMPONENT_REGISTER, this });
				registered = true;
			}

			void Component::setup(GameObject* go)
			{
				_gameObject = go;
			}
		}
	}
}
