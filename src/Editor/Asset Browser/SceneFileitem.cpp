#ifdef TRISTEON_EDITOR

#include "SceneFileItem.h"
#include "Core/Scene.h"
#include "Core/SceneManager.h"

using namespace Tristeon::Editor;
using namespace Tristeon;

REGISTER_TYPE_CPP(SceneFileItem)

SceneFileItem::SceneFileItem()
{
}

void SceneFileItem::createFile(nlohmann::json json)
{
	AssetItem::createFile(json);
	Core::SceneManager::addScenePath(name,getFilePath());
}

void SceneFileItem::onDoubleClick()
{
	std::cout << "Loading scene: " << name << std::endl;
	Core::SceneManager::load(name);
}

void SceneFileItem::removeFile()
{
	AssetItem::removeFile();
	Core::SceneManager::removeScenePath(name);
}

nlohmann::json SceneFileItem::serialize()
{
	nlohmann::json output;
	output["typeID"] = Tristeon::Core::Type<SceneFileItem>::fullName();
	output["isFolder"] = isFolder;
	output["filepath"] = filepath;
	output["name"] = name;
	output["extension"] = extension;
	return output;
}

void SceneFileItem::deserialize(nlohmann::json json)
{
	isFolder = json["isFolder"];
	const std::string filepathValue = json["filepath"];
	filepath = filepathValue;
	const std::string nameValue = json["name"];
	name = nameValue;
	const std::string extensionValue = json["extension"];
	extension = extensionValue;
}

#endif
