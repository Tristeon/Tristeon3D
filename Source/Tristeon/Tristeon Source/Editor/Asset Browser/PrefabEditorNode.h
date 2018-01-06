#pragma once
#include "Editor/EditorNode.h"
#include "PrefabFileItem.h"

class PrefabEditorNode : public Tristeon::Editor::EditorNode
{
public:
	nlohmann::json getPrefabData();
	void applyPrefab();
private:
	PrefabFileItem* prefab;
};
