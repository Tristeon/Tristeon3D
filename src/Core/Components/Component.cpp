#include "Component.h"
#include "Core/Collector.h"
#include "Core/GameObject.h"

namespace Tristeon::Core::Components
{
	Component::Component()
	{
		Collector<Component>::add(this);
	}

	Component::~Component()
	{
		Collector<Component>::remove(this);
	}

	Component::property__tmp_type_transform Component::get_transform()
	{
		return _gameObject->transform.get();
	}

	void Component::setup(GameObject* go)
	{
		_gameObject = go;
	}
}
