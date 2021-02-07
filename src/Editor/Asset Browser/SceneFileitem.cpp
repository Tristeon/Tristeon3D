#ifdef TRISTEON_EDITOR

#include "SceneFileItem.h"
#include "Scenes/Scene.h"
#include "Scenes/SceneManager.h"

using namespace Tristeon::Editor;
using namespace Tristeon;

REGISTER_TYPE_CPP(SceneFileItem)

SceneFileItem::SceneFileItem()
{
}

void SceneFileItem::createFile(nlohmann::json json)
{
	AssetItem::createFile(json);
	Scenes::SceneManager::addScenePath(name,getFilePath());
}

void SceneFileItem::onDoubleClick()
{
	std::cout << "Loading scene: " << name << std::endl;
	Scenes::SceneManager::loadScene(name);
}

void SceneFileItem::removeFile()
{
	AssetItem::removeFile();
	Scenes::SceneManager::removeScenePath(name);
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
