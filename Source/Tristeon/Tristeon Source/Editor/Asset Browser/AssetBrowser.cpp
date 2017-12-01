#include "AssetBrowser.h"
#include "FileItemManager.h"
#include "FolderItem.h"
#include <fstream>
#include <GLFW/glfw3.h>
#include "SceneFileItem.h"
#include "Scenes/Scene.h"
#include "Scenes/SceneManager.h"
#include "Editor/JsonSerializer.h"

using namespace Tristeon::Editor;
using namespace Tristeon;

const std::string AssetBrowser::assetPath = "Assets/";

AssetBrowser::AssetBrowser()
{
	//Setup rootfolder
	rootFolder = std::make_shared<FolderItem>();
	rootFolder->filepath = "Assets";
	rootFolder->isFolder = true;
	rootFolder->setup(true);

	//Set asset browser view to the asset folder (rootfolder)
	itemManager = std::make_shared<FileItemManager>();
	itemManager->setView(rootFolder.get());
}

AssetBrowser::~AssetBrowser()
{
}

void AssetBrowser::onGui()
{
	//On CTL+S
	if (ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && ImGui::IsKeyPressed(GLFW_KEY_S,false))
	{
		//Save current scene
		if (Scenes::SceneManager::getActiveScene() != nullptr) {
			Scenes::Scene* currentScene = Scenes::SceneManager::getActiveScene();
			std::cout << "Saving scene: " << currentScene->name << std::endl;
			std::cout << "First gameObject name: " << currentScene->gameObjects[0]->name << std::endl;
			if (itemManager->currentlyLoadedSceneFile != nullptr)
				itemManager->currentlyLoadedSceneFile->createSceneFile(*currentScene);
			else
			{
				//Create a scene file
				SceneFileItem* sceneFile = new SceneFileItem();
				sceneFile->createItem("Scene", itemManager->currentFolder, "scene"); //Create meta data
				sceneFile->createSceneFile(*currentScene); //Create create the scene file
			}
		}
	}

	ImGui::Begin("Tristeon file window",0,windowFlags); // begin window

	//Draw folder hierarchy
	ImGui::BeginChild("Left panel", ImVec2(150, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
	folderHierarchy.drawHierarchy(rootFolder,itemManager);
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("Right panel");
	// Right click popup (opens asset creation window)
	if (ImGui::BeginPopupContextWindow("Asset creation menu")) {

		//TODO: Select desired asset type to create and then create name after fileitem is created
		//		aka remove hard-code
		// Asset creation UI
		ImGui::InputText("Asset name", createdItemName, 255);

		if (std::string(createdItemName).size() < 24)
		{
			if (ImGui::Button("Create folder") || ImGui::IsKeyPressed(GLFW_KEY_ENTER,false))
			{
				FolderItem* folder = new FolderItem();
				folder->createItem(createdItemName, itemManager->currentFolder); //Create both folder and meta data
				//Set parenting and child relations
				folder->parent = itemManager->currentFolder;
				itemManager->currentFolder->children.push_back(folder);
				itemManager->currentFolder->fileItems.push_back(folder);
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::Button("Create scene") || ImGui::IsKeyPressed(GLFW_KEY_ENTER,false))
			{
				//Create a scene file
				SceneFileItem* sceneFile = new SceneFileItem();
				sceneFile->createItem(createdItemName,itemManager->currentFolder,"scene"); //Create meta data
				Scenes::Scene createdScene = Scenes::Scene(); //Create empty scene
				sceneFile->createSceneFile(createdScene); //Create create the scene file
				ImGui::CloseCurrentPopup();
			}
		}
		else std::cout << "Can't create items with more than 24 characters\n";
		ImGui::EndPopup();
	}
	
	itemManager->drawFileItems();
	ImGui::EndChild();

	ImGui::End(); // end window
}
