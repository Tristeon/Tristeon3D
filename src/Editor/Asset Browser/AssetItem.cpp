#ifdef TRISTEON_EDITOR

#include <filesystem>
#include "AssetItem.h"
#include "FolderItem.h"
#include "Core/JsonSerializer.h"

namespace filesystem = std::filesystem;
using namespace Tristeon::Editor;

REGISTER_TYPE_CPP(AssetItem)

AssetItem::AssetItem()
{
	std::cout << "Created unrecognized fileitem\n";
}

AssetItem::~AssetItem()
{
}

nlohmann::json AssetItem::serialize()
{
	nlohmann::json output = FileItem::serialize();
	output["typeID"] = Core::Type<AssetItem>::fullName();
	output["extension"] = extension;
	return output;
}

void AssetItem::deserialize(nlohmann::json json)
{
	isFolder = json["isFolder"];
	const std::string filepathValue = json["filepath"];
	filepath = filepathValue;
	const std::string nameValue = json["name"];
	name = nameValue;
	const std::string extensionValue = json["extension"];
	extension = extensionValue;
	if (json["GUID"].is_string())
	{
		const std::string GUIDValue = json["GUID"];
		GUID = GUIDValue;
	}
}

void AssetItem::init(std::string name, FolderItem* folder, std::string extension)
{
	//Set filepath
	std::string filePath = folder->filepath + folder->name + "/";

	//If the user entered an empty space generate new folder
	if (name == "" || name == " ") name = "New folder";

	//TODO: move this code outside of the creation

	////Check if file already exists
	//if (FileManager::fileExists(filePath + name + ".meta"))
	//{
	//	//Explicitly add a number to the new folder (eg. if Prefab.meta exists create Prefab 2.meta)
	//	int index = 1;
	//	while (FileManager::fileExists(filePath + name + " " + std::to_string(index) + ".meta"))
	//	{
	//		index++;
	//	}
	//	//Change name to an unique one
	//	name += " " + std::to_string(index);
	//}

	this->name = name;
	this->extension = extension;
	this->filepath = filePath;

	//Set parenting and child relations
	parent = folder;
	parent->fileItems.push_back(this);
	Core::JsonSerializer::serialize(filepath + name + "." + extension + ".meta", *this);
}

void AssetItem::createFile(nlohmann::json json)
{
	Core::JsonSerializer::serialize(filepath + name + "." + extension, json);
}

void AssetItem::move(FolderItem* destination)
{
	std::string destinationFilepath = destination->filepath + destination->name + "/" + name + "." + extension;
	if (filesystem::exists(destinationFilepath))
	{
		std::cout << "Can't move " << name << " to folder " << destination->name << std::endl;
		return;
	}

	//Remove file reference in the old parent
	const auto removeIterator = remove(parent->fileItems.begin(), parent->fileItems.end(), this);
	parent->fileItems.erase(removeIterator, parent->fileItems.end());

	//Set new parent
	parent = destination;

	std::string p1 = filepath + name + "." + extension;
	std::string p2 = destinationFilepath;

	//Copy file
	copy_file(p1,p2, filesystem::copy_options::overwrite_existing);
	//Remove old file and meta file
	filesystem::remove(p1);
	filesystem::remove(p1 + ".meta");

	//Create new fileitem metadata
	init(name, destination, extension);
}

void AssetItem::removeFile()
{
	//Remove original file and meta file
	filesystem::remove(filepath + name + "." + extension);
	filesystem::remove(filepath + name + "." + extension + ".meta");
}

void AssetItem::onDoubleClick()
{
	//Do nothing
}

std::string AssetItem::getFilePath() const
{
	return filepath + name + "." + extension;
}

void AssetItem::drawOnInspector()
{
	//Do nothing
}

#endif
