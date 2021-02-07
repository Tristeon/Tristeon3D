#ifdef TRISTEON_EDITOR

#include <filesystem>
#include "FolderItem.h"
#include "FileItemManager.h"
#include "AssetItem.h"
#include "Editor/JsonSerializer.h"
#include "Misc/StringUtils.h"
#include "Misc/Console.h"
#include "Editor/EditorDragging.h"
#include "MeshFileItem.h"
#include <assimp/Importer.hpp>
#include "XPlatform/typename.h"

using namespace std;
using namespace Tristeon::Editor;

DerivedRegister<FolderItem> FolderItem::reg;

FolderItem::FolderItem()
{
	isFolder = true;
	selectionMask = 0;
}

FolderItem::~FolderItem()
{
	//Remove child reference in fileitems vector
	for (unsigned int i = 0; i < children.size(); ++i)
	{
		const auto removeIterator = std::remove(fileItems.begin(), fileItems.end(), children[i].get());
		fileItems.erase(removeIterator);
	}

	//Clean up children, unique pointers automatically call the destructor
	children.clear();

	//Clean fileitems
	for (unsigned int i = 0; i < fileItems.size(); ++i)
		delete fileItems[i];
}

void FolderItem::init(string name, FolderItem* folder, string extension)
{
	//Set filepath
	string filePath = folder->filepath+folder->name+"/";

	//If the user entered an empty space generate new folder
	if (name == "" || name == " ") name = "New folder";

	//Check if file already exists
	if (std::filesystem::exists(filePath + name + ".meta"))
	{
		//Explicitly add a number to the new folder (eg. if Prefab.meta exists create Prefab 2.meta)
		int index = 1;
		while (filesystem::exists(filePath + name + " " + to_string(index) + ".meta"))
		{
			index++;
		}
		//Change name to an unique one
		name += " " + to_string(index);
	}

	this->name = name;
	this->filepath = filePath;
	JsonSerializer::serialize(filepath + name + ".meta", *this);
	filesystem::create_directory(string(filepath + name).c_str());
}

/**
 Creates metadata of a file that is found using the given filepath
 */
void FolderItem::createMetaData(const string& filepath)
{
	//TODO: implement identification of the file where metadata is created for, so instead of assuming that
	//if it's not a folder that it's simply an assetitem, further define what that assetitem could be.
	FileItem* itemMetaData;
	filesystem::path path(filepath);
	if (filesystem::is_directory(filepath))
		itemMetaData = new FolderItem();
	else
	{
		Assimp::Importer imp;
		if(imp.IsExtensionSupported(path.extension().string()))
		{
			itemMetaData = new MeshFileItem();
		}
		itemMetaData = new AssetItem();
	}

	//TODO: cleanup code and make it more clear
	//Seperate the path by folders
	vector<string> splitString = StringUtils::split(filepath,'/');
	//Get the filename by getting the last file in the filepath (Also has file extension)
	string filename = splitString[splitString.size() - 1];
	if (itemMetaData->isFolder)
		itemMetaData->init(filename, this);
	else
		itemMetaData->init(StringUtils::split(filename, '.')[0] /*Get the name without extension*/, this,
			StringUtils::split(filename, '.')[1]/*Get only the extension*/);
}

void FolderItem::setup(bool doChildren)
{
	//TODO: change all string manipulation of filepaths and use the path class instead
	vector<string> metaFiles;
	vector<string> filesInFolder;
	map<string, string> metadata;

	//Reset view
	fileItems.clear();
	children.clear();

	//Find all files inside of this folder and store their filepath inside of `filesInFolder` if they're a file
	//and if they're a meta file add it to metadata
	for (auto& file : filesystem::directory_iterator(filepath+name+"/"))
	{
		filesystem::path filePath = file.path();
		string filePathGenericString = filePath.generic_string();
		//If it found a meta file
		if (filePath.extension() == ".meta") {
			string originalFilePath = filePathGenericString.substr(0, filePathGenericString.size() - 5);
			if (!filesystem::exists(originalFilePath))
			{
				Misc::Console::warning("A meta data file exists but its asset '" + originalFilePath + "' can't be found. When moving or deleting files outside of Tristeon, please ensure that the corresponding .meta file is moved or deleted along with it.");
			}
			metaFiles.push_back(filePathGenericString);
			metadata[originalFilePath] = "Has meta file";
			Misc::Console::write("Found meta file: " + filePathGenericString);
		}
		else
		{
			Misc::Console::write("Found file: " + filePathGenericString);
			filesInFolder.push_back(filePathGenericString);
		}
	}

	//Iterate through all the found files inside of the folder and find the meta data
	for (string filepath : filesInFolder)
	{
		//If file doesn't have a meta file
		if (metadata[filepath] == "")
		{
			cout << "File: "<<filepath<<" does not have a meta file\n";
			createMetaData(filepath);
			cout << "Adding metadata for " << filepath << endl;
		}
	}
	for (string metafilepath : metaFiles)
    {
		FileItem* fileItemToAdd = nullptr;
		ifstream stream(metafilepath);
		if (!stream.good()) {
			cout << "Serializer can't serialize cus can't read file: " << filepath << "\n";
			continue;
		}

		nlohmann::json input;
		stream >> input;
		auto deserializedObject = TypeRegister::createInstance(input["typeID"]);

		fileItemToAdd = (FileItem*) deserializedObject.get();
		deserializedObject.release();
		fileItemToAdd->parent = this;

		//Load meta data into the fileitem
		fileItemToAdd->deserialize(input);
		if (fileItemToAdd->isFolder)
		{
			FolderItem* folder = (FolderItem*)fileItemToAdd;
			children.push_back(std::move(std::unique_ptr<FolderItem>(folder)));
			folder->parent = this;
			if (doChildren && !folder->setupDone) folder->setup(doChildren);
		}

		fileItems.push_back(fileItemToAdd);
	}

	//finished setup
	setupDone = true;
}

void FolderItem::drawHierarchy(FileItemManager* itemManager)
{
	int nodeClicked = -1;	// Temporary storage of what node we have clicked to process selection at the end of the loop
	for (int i = 0; i < children.size(); i++)
	{
		// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick
		| (children[i]->children.size() == 0 ? ImGuiTreeNodeFlags_Leaf : 0);
		const bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)i, nodeFlags, children[i]->name.c_str());

		//TODO: Improve drag'n'drop by manually selecting the node and manually checking collision
		if (dynamic_cast<FileItem*>(EditorDragging::getDragableItem()) != nullptr && ImGui::IsItemHovered())
		{
			FileItem* draggingItem = dynamic_cast<FileItem*>(EditorDragging::getDragableItem());
			cout << "Moving item: " << draggingItem->name << " to " << children[i]->name << endl;
			if (draggingItem->parent != children[i].get())
				draggingItem->move(children[i].get());
			else cout << "Check this code please... dafuq is going on\n"; //TODO: move if maybe statement maybe?
		}
		if (ImGui::IsItemClicked())
			nodeClicked = i;
		if (nodeOpen)
		{
			children[i]->drawHierarchy(itemManager);
			ImGui::TreePop();
		}
	}
	if (nodeClicked > -1)
	{
		itemManager->setView(children[nodeClicked].get());
	}
}

nlohmann::json FolderItem::serialize()
{
	nlohmann::json output = FileItem::serialize();
	output["typeID"] = TRISTEON_TYPENAME(FolderItem);
	return output;
}

void FolderItem::deserialize(nlohmann::json json)
{
	FileItem::deserialize(json);
}

void FolderItem::removeFile()
{
	filesystem::remove(filepath + name);
	filesystem::remove(filepath + name + ".meta");
}

bool FolderItem::hasChild(FolderItem* destination)
{
	if (this != destination)
	{
		bool childFound = false;
		for (int i = 0; i < children.size(); ++i)
		{
			childFound = children[i]->hasChild(destination);
		}
		return childFound;
	}
	return true;
}

void FolderItem::move(FolderItem* destination)
{
	if (this != destination && !hasChild(destination) && parent != destination)
	{
		//Remove folder reference in the old parent
		for (int i = 0; i < parent->children.size(); ++i)
		{
			if (parent->children[i].get() == this)
			{
				parent->children.erase(parent->children.begin() + i);
				break;
			}
		}

		//Remove file reference in the old parent
		const auto removeIterator = remove(parent->fileItems.begin(), parent->fileItems.end(), this);
		parent->fileItems.erase(removeIterator, parent->fileItems.end());

		//Set new parent
		parent = destination;
		parent->children.push_back(std::move(std::unique_ptr<FolderItem>(this)));
		parent->fileItems.push_back(this);
		//Cache old filepath
		string oldFilepath = filepath;

		//Create new folder
		init(name,destination);

		//Move children
		for (FileItem* childItem : fileItems)
		{
			if (!childItem->isFolder) childItem->move(destination);
			else childItem->move(this);
		}

		//Delete old folder & meta data
		filesystem::remove_all(oldFilepath +name);
		filesystem::remove(oldFilepath + name + ".meta");
	}
	else
	{
		cout << "Can't drag folder: " << name << " to " << destination->name << endl;
	}
}

#endif
