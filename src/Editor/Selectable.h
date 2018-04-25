#pragma once
#if TRISTEON_EDITOR

namespace Tristeon
{
	namespace Editor
	{
		/**
		 * \brief Base class for all the editor items that can be selected
		 * eg. inspector displays a window depending on what item is selected
		 */
		class Selectable
		{
		protected:
			Selectable() = default;
			virtual ~Selectable() = default;
		};
	}
}

#endif