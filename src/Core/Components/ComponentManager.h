#pragma once
#include <Core/Components/Component.h>
#include <Core/Collector.h>

namespace Tristeon::Core::Components
{
	/**
	 * ComponentManager keeps track of existing components and runs callbacks on them when needed.
	 * ComponentManager implements a basic (de)register system that listens to the initailization of new components.
	 *
	 * This class is not intended to be accessed or used by users.
	 */
	class ComponentManager final
	{
	public:
		ComponentManager();

	private:
		/**
		 * Calls function f on every registered component
		 */
		template <void (Component::*func)()>
		void callFunction();
	};

	template <void(Component::*func)()>
	void ComponentManager::callFunction()
	{
		for (Component* c : Collector<Component>::all())
			(c->*func)();
	}
}