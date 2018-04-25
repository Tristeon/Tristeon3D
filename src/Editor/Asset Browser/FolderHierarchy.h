#pragma once
#if TRISTEON_EDITOR

#include <memory>

namespace Tristeon
{
	namespace Editor
	{
		class FolderItem;
		class FileItemManager;

		/**
		 * \brief Responsible of drawing the folder hierarchy of the rootfolder
		 */
		class FolderHierarchy
		{
		public:
			FolderHierarchy();
			~FolderHierarchy();
			/**
			 * \brief Draws folder structure of rootfolder
			 */
			void drawHierarchy(FolderItem* rootFolder, FileItemManager* itemManager);
		private:
			int selected = 0;
		};
	}
}

#endif