#pragma once
#include <vector>
#include "../ImGUI/imgui.h"
#include "FileItem.h"
#include "SceneFileitem.h"

namespace Tristeon
{
	namespace Editor
	{
		class FolderItem;

		/**
		 * \brief FileItemManager is responsible for displaying the contents of a folder using icons (icons are still W.I.P.)
		 */
		class FileItemManager
		{
		public:
			FileItemManager() = default;
			~FileItemManager();

			/**
			 * \brief Size of the selectable icons it displays
			 */
			ImVec2 iconSize = ImVec2(50, 50);

			/**
			 * \brief The fileitems that have been selected
			 */
			std::vector<bool> highlightedFileItems = std::vector<bool>();

			/**
			 * \brief Draws the fileitems in the currentfolder
			 */
			void drawFileItems();

			/**
			 * \brief Changes the currently viewed folder to the new specified one
			 */
			void setView(FolderItem* folder);

			/**
			 * \brief The folder that is used to display the fileitems
			 */
			FolderItem* currentFolder = nullptr;

			/**
			 * \brief The latest scene that has been loaded through the assetbrowser
			 */
			SceneFileItem* currentlyLoadedSceneFile;
		};
	}
}
