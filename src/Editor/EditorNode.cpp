#ifdef TRISTEON_EDITOR

#include "EditorNode.h"
#include "Core/JsonSerializer.h"

using namespace Tristeon::Editor;


EditorNode::EditorNode(Tristeon::Core::GameObject* gameObj)
{
	connectedGameObject = gameObj;
	_isPrefab = false;
}

EditorNode::EditorNode(Tristeon::Core::GameObject* gameObj, std::string prefabFilePath)
{
	connectedGameObject = gameObj;
	setPrefab(prefabFilePath);
}

EditorNode::~EditorNode()
{
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
		connectedGameObject->transform.get()->setParent(parent->connectedGameObject->transform.get(), false);
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
	return Core::JsonSerializer::load(connectedGameObject->prefabFilePath);
}

void EditorNode::applyPrefab()
{
	data = getPrefabData();
	applyChanges();
}

void EditorNode::setParent(EditorNode* editorNode)
{
	parent = editorNode;
	editorNode->children.push_back(this);
	connectedGameObject->transform.get()->setParent(editorNode->connectedGameObject->transform.get());
}

void EditorNode::applyChanges()
{
	connectedGameObject->deserialize(*getData());
}

void EditorNode::setPrefab(std::string filepath)
{
	std::ifstream stream(filepath);
	_isPrefab = filepath != "" && stream.good();
	connectedGameObject->prefabFilePath = filepath;
	data = connectedGameObject->serialize();
}

void EditorNode::setPrefabData(nlohmann::json prefabData)
{
	Core::JsonSerializer::serialize(connectedGameObject->prefabFilePath,prefabData);
}

#endif