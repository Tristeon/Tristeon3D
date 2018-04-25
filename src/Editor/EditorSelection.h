#pragma once
#if TRISTEON_EDITOR

#include "Selectable.h"

namespace Tristeon
{
	namespace Editor
	{
		/**
		 * \brief It manages all selectable objects to keep track of what the user has selected
		 */
		class EditorSelection
		{
		public:
			/**
			 * \return Returns the current selectedItem. Any EditorNode type object can be selected
			 * but selection is handled manually through the static void setSelectedItem()
			 */
			static Selectable* getSelectedItem();
			/**
			 * \param node The node to be set as the selectedItem
			 */
			static void setSelectedItem(Selectable* node);
		private:
			static Selectable* selectedItem;
		};
	}
}

#endif