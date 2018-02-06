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

	//Images
	assetIcons["jpg"] = std::make_unique<EditorImage>("Files/Editor/Icons/Image.png", bindingData);
	assetIcons["png"] = std::make_unique<EditorImage>("Files/Editor/Icons/Image.png", bindingData);
	assetIcons["tga"] = std::make_unique<EditorImage>("Files/Editor/Icons/Image.png", bindingData);
	assetIcons["bmp"] = std::make_unique<EditorImage>("Files/Editor/Icons/Image.png", bindingData);
	assetIcons["psd"] = std::make_unique<EditorImage>("Files/Editor/Icons/Image.png", bindingData);

	//Materials
	assetIcons["mat"] = std::make_unique<EditorImage>("Files/Editor/Icons/Material.png", bindingData);
	assetIcons["shader"] = std::make_unique<EditorImage>("Files/Editor/Icons/Shader.png", bindingData);
	assetIcons["skybox"] = std::make_unique<EditorImage>("Files/Editor/Icons/Skybox.png", bindingData);
	
	//Mesh files
	assetIcons["fbx"] = std::make_unique<EditorImage>("Files/Editor/Icons/Mesh.png", bindingData);
	assetIcons["obj"] = std::make_unique<EditorImage>("Files/Editor/Icons/Mesh.png", bindingData);
	assetIcons["3ds"] = std::make_unique<EditorImage>("Files/Editor/Icons/Mesh.png", bindingData);
	assetIcons["blend"] = std::make_unique<EditorImage>("Files/Editor/Icons/Mesh.png", bindingData);

	//Other
	assetIcons["text"] = std::make_unique<EditorImage>("Files/Editor/Icons/Text.png", bindingData);

	//Programming
	assetIcons["cpp"] = std::make_unique<EditorImage>("Files/Editor/Icons/Text.png", bindingData);
	assetIcons["c"] = std::make_unique<EditorImage>("Files/Editor/Icons/Text.png", bindingData);
	assetIcons["h"] = std::make_unique<EditorImage>("Files/Editor/Icons/Text.png", bindingData);
	assetIcons["hpp"] = std::make_unique<EditorImage>("Files/Editor/Icons/Text.png", bindingData);

	//Audio
	assetIcons["mp3"] = std::make_unique<EditorImage>("Files/Editor/Icons/Audio.png", bindingData);
	assetIcons["wav"] = std::make_unique<EditorImage>("Files/Editor/Icons/Audio.png", bindingData);
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
		if (fileItem == nullptr)
		{
			Misc::Console::warning("Fileitem is null in FileItemManager::drawFileItems!");
			continue;
		}

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
				for (unsigned int j = 0; j < currentFolder->children.size(); ++j)
				{
					if (currentFolder->children[j].get() == (FolderItem*)fileItem)
					{
						currentFolder->children.erase(currentFolder->children.begin() + j);
						break;
					}
				}
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
