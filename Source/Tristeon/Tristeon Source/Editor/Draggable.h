#pragma once
namespace Tristeon
{
	namespace Editor
	{
		/**
		 * \brief Base class for all the editor items that can be dragged
		 * eg. a prefab gets dragged into a inspector property field.
		 */
		class Draggable
		{
		protected:
			Draggable() = default;
			virtual ~Draggable() = default;
		};
	}
}
