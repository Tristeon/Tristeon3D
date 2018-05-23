#pragma once
#ifdef TRISTEON_EDITOR

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
		public:
			virtual ~Draggable() = default;
		};
	}
}

#endif