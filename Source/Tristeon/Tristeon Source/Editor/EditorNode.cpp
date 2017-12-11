#include "EditorNode.h"

using namespace Tristeon::Editor;


EditorNode::EditorNode(Tristeon::Core::GameObject* gameObj)
{
	connectedGameObject = gameObj;
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

void EditorNode::applyChanges()
{
	connectedGameObject->deserialize(*getData());
}
