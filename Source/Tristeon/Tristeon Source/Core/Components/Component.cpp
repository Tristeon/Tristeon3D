#include "Component.h"
#include "Core/ManagerProtocol.h"
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
					ManagerProtocol::sendMessage({ MT_SCRIPTINGCOMPONENT_DEREGISTER, this });
			}

			Core::Transform* Component::property__get_transform() const
			{
				return gameObject->transform;
			}

			void Component::init()
			{
				if (!registered)
					ManagerProtocol::sendMessage({ MT_SCRIPTINGCOMPONENT_REGISTER, this });
				registered = true;
			}

			void Component::setup(GameObject* go)
			{
				_gameObject = go;
			}
		}
	}
}
