#pragma once
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
			void drawHierarchy(std::shared_ptr<FolderItem> rootFolder, std::shared_ptr<FileItemManager> itemManager);
		private:
			int selected = 0;
		};
	}
}
