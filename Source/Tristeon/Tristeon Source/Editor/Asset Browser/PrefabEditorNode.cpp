#include "PrefabEditorNode.h"
#include "Editor/JsonSerializer.h"

nlohmann::json PrefabEditorNode::getPrefabData()
{
	return JsonSerializer::load(prefab->getFilePath());
}

void PrefabEditorNode::applyPrefab()
{
	data = getPrefabData();
}
