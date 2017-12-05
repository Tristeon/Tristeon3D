#include "FileItemManager.h"
#include <windows.h>
#include <filesystem>
#include "FolderItem.h"
#include <GLFW/glfw3.h>
#include "SceneFileItem.h"
#include "Scenes/Scene.h"
#include "Editor/EditorDragging.h"

using namespace std;
using namespace Tristeon::Editor;
namespace fs = experimental::filesystem;
using Scene = Tristeon::Scenes::Scene;

FileItemManager::~FileItemManager()
{
	delete currentFolder;
}

void FileItemManager::drawFileItems()
{
	if (currentFolder == nullptr) return;
	//Loading fileitems
	for (int i = 0; i < currentFolder->fileItems.size(); ++i)
	{
		ImGui::PushID(i);
		FileItem* fileItem = currentFolder->fileItems[i];

		if (currentFolder->fileItems.size() > highlightedFileItems.size())
			highlightedFileItems.push_back(false);

		//Load file item UI
		bool selectablePressed = ImGui::Selectable(fileItem->name.c_str(), highlightedFileItems[i], 0, iconSize);
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) EditorDragging::setDragableItem(fileItem);

		//When delete is pressed
		if (highlightedFileItems[i] && ImGui::IsKeyPressed(GLFW_KEY_DELETE, false))
		{
			//Remove references in the containing folder
			const auto removeIterator = remove(currentFolder->fileItems.begin(), currentFolder->fileItems.end(), fileItem);
			currentFolder->fileItems.erase(removeIterator, currentFolder->fileItems.end());

			if (fileItem->isFolder)
			{
				const auto removeFolderIterator = remove(currentFolder->children.begin(), currentFolder->children.end(), (FolderItem*)fileItem);
				currentFolder->children.erase(removeFolderIterator, currentFolder->children.end());
			}
			ImGui::PopID();
			continue;
		}

		if (selectablePressed)
		{
			//Is left control held?
			if (ImGui::GetIO().KeyCtrl)
				highlightedFileItems[i] = !highlightedFileItems[i];
			else
			{
				highlightedFileItems = std::vector<bool>(currentFolder->fileItems.size());
				highlightedFileItems[i] = true;
			}
		}

		//When selectable is double clicked
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
		{
			//What kind of item has been double clicked?
			if (fileItem->isFolder) setView((FolderItem*)fileItem);
			else if (fileItem->isType<SceneFileItem>())
			{
				SceneFileItem* sceneFileItem = (SceneFileItem*)fileItem;
				sceneFileItem->onDoubleClick();
				currentlyLoadedSceneFile = sceneFileItem;
			}
		}

		//Changes rows and columns of the item layout to fit the window
		const float columnWidth = ImGui::GetWindowContentRegionWidth() - 45;
		if (iconSize.x) {
			int horizontalFolderAmount = columnWidth / iconSize.x;
			if (horizontalFolderAmount == 0) horizontalFolderAmount += 1;
			if (i % horizontalFolderAmount < horizontalFolderAmount - 1) ImGui::SameLine();
		}
		ImGui::PopID();
	}
}

void FileItemManager::setView(FolderItem* folder)
{
	currentFolder = folder;
}
