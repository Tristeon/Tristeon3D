#include "SceneFileItem.h"
#include "Editor/JsonSerializer.h"
#include "Scenes/Scene.h"
#include "Scenes/SceneManager.h"

using namespace Tristeon::Editor;
using namespace Tristeon;

REGISTER_TYPE_CPP(SceneFileItem)

SceneFileItem::SceneFileItem()
{
}

void SceneFileItem::onDoubleClick()
{
	std::cout << "Loading scene: " << name << std::endl;
	std::string sceneFilePath = filepath + name + "." + extension;
	auto scene = JsonSerializer::deserialize<Scenes::Scene>(sceneFilePath);
	//Can't pass the shared_ptr's pointer because the shared_ptr will delete the value. So it must copied to relieve ownership
	Scenes::SceneManager::loadScene(scene);
}