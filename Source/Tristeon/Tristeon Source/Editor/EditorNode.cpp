#include "EditorNode.h"
#include "JsonSerializer.h"

using namespace Tristeon::Editor;


EditorNode::EditorNode(Tristeon::Core::GameObject* gameObj)
{
	connectedGameObject = gameObj;
	_isPrefab = false;
}

EditorNode::EditorNode(Tristeon::Core::GameObject* gameObj, PrefabFileItem* prefab)
{
	connectedGameObject = gameObj;
	this->prefab = prefab;
	_isPrefab = true;
}

EditorNode::~EditorNode()
{
	for (auto child : children)
	{
		delete child;
	}
}

void EditorNode::load(nlohmann::json json)
{
	data = json;
	const std::string typeIDValue = json["typeID"]; //Regarding a change in C++17 the library no longer can directly convert into a string as it returns an ambiguous error
	typeID = typeIDValue;
}

nlohmann::json* EditorNode::getData()
{
	return &data;
}

void EditorNode::move(EditorNode * parent)
{
	if (parent == this->parent)
	{
		std::cout << "Ur trying to move an editor node to the parent that it already has\n";
		return;
	}

	if (this->parent != nullptr)
	{
		//Remove old parent relation
		auto removeIterator = std::remove(this->parent->children.begin(), this->parent->children.end(), this);
		this->parent->children.erase(removeIterator);
	}

	//Set new parent
	if (parent != nullptr) {
		parent->children.push_back(this);

		//Change gameObject's relations
		applyChanges();
		connectedGameObject->transform->setParent(parent->connectedGameObject->transform);
		nlohmann::json serializedGameObject = connectedGameObject->serialize();
	}
	this->parent = parent;

}

bool EditorNode::hasChild(EditorNode* node)
{
	if (this != node)
	{
		bool childFound = false;
		for (EditorNode* child : children)
		{
			childFound = child->hasChild(node);
		}
		return childFound;
	}
	return true;
}

nlohmann::json EditorNode::getPrefabData()
{
	return JsonSerializer::load(prefab->getFilePath());
}

void EditorNode::applyPrefab()
{
	data = getPrefabData();
	applyChanges();
}

void EditorNode::applyChanges()
{
	connectedGameObject->deserialize(*getData());
}

void EditorNode::setPrefab(PrefabFileItem* prefab)
{
	_isPrefab = prefab != nullptr;
	this->prefab = prefab;
}

void EditorNode::setPrefabData(nlohmann::json prefabData)
{
	prefab->deserialize(prefabData);
}
