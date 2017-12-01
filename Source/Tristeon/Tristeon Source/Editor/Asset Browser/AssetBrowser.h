#pragma once
#include "FileItemManager.h"
#include "FolderHierarchy.h"
#include "FolderItem.h"
#include "Editor/EditorWindow.h"

namespace Tristeon
{
	namespace Editor
	{
		/**
		 * \brief The assetbrowser is a window that allows the user to create new assets and manage their assets
		 * in the project, create prefabs, etc.
		 */
		class AssetBrowser : public EditorWindow
		{
		public:
			/**
			 * \brief The filepath of the asset folder
			 */
			static const std::string assetPath;
			AssetBrowser();
			~AssetBrowser();
			/**
			* \brief onGUI is called for rendering and handling GUI events.
			*		Currently onGUI is called per-frame however that might be subject to change
			*/
			void onGui() override;
		private:
			char createdItemName[255] = "";
			/**
			 * \brief The item manager is in charge of displaying the fileitems of the current folder that has been
			 * opened in the asset browser and managing their interactions
			 */
			std::shared_ptr<FileItemManager> itemManager;
			/**
			 * \brief The folder hierarchy is in charge of drawing the folder structure of the asset folder
			 */
			FolderHierarchy folderHierarchy;

			std::shared_ptr<FolderItem> rootFolder;
		};
	}
}
