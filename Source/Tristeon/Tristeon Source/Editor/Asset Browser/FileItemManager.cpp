#include "FileItemManager.h"
#include <filesystem>
#include "FolderItem.h"
#include <GLFW/glfw3.h>
#include "SceneFileItem.h"
#include "Scenes/Scene.h"
#include "Editor/EditorDragging.h"
#include "Editor/EditorSelection.h"

using namespace std;
using namespace Tristeon::Editor;
namespace fs = experimental::filesystem;
using Scene = Tristeon::Scenes::Scene;

FileItemManager::FileItemManager(Tristeon::Core::BindingData* bindingData)
{
	folderIcon = std::make_unique<EditorImage>("Files/Editor/Icons/Folder.png", bindingData);
	standardIcon = std::make_unique<EditorImage>("Files/Editor/Icons/Standard.png", bindingData);
	assetIcons["scene"] = std::make_unique<EditorImage>("Files/Editor/Icons/Scene.png", bindingData);
}

FileItemManager::~FileItemManager()
{
}

void FileItemManager::drawFileItems()
{
	if (currentFolder == nullptr) return;
	//Loading fileitems
	for (unsigned int i = 0; i < currentFolder->fileItems.size(); ++i)
	{
		ImGui::PushID(i);
		FileItem* fileItem = currentFolder->fileItems[i];

		if (currentFolder->fileItems.size() > highlightedFileItems.size())
			highlightedFileItems.push_back(false);

		//Load file item UI
		ImTextureID iconTexture;

		//Find right icon to display for the
		if (fileItem->isFolder)
			iconTexture = folderIcon->getTextureID();
		else
		{
			//Do we have an icon for this file type?
			if (assetIcons.find(((AssetItem*)fileItem)->extension) != assetIcons.end())
				iconTexture = assetIcons[((AssetItem*)fileItem)->extension]->getTextureID();
			else iconTexture = standardIcon->getTextureID();
		}
		
		bool selectablePressed = ImGui::TSelectable(fileItem->name.c_str(), highlightedFileItems[i], 0, iconSize, iconTexture);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && ImGui::IsMouseClicked(0)) EditorDragging::setDragableItem(fileItem);

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
			//If the selected fileitem is an assetitem
			AssetItem* assetItem = dynamic_cast<AssetItem*>(fileItem);
			if (assetItem != nullptr)
			{
				EditorSelection::setSelectedItem(assetItem);
			}
		}

		//When selectable is double clicked
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && ImGui::IsMouseDoubleClicked(0))
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
