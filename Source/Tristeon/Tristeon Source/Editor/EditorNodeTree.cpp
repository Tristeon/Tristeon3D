#include "EditorNodeTree.h"
#include "Scenes/SceneManager.h"

using namespace Tristeon::Editor;

Tristeon::Editor::EditorNodeTree::~EditorNodeTree()
{
	for (EditorNode* node : nodes) {
		delete node;
	}
}

void EditorNodeTree::load(nlohmann::json nodeTree)
{
	nodes.clear();
	if (nodeTree.is_array())
	{
		for (auto iterator = nodeTree.begin(); iterator != nodeTree.end(); ++iterator)
		{
			//TODO: implement a faster implementation for loading scenes, this could be very performant intensive in big scenes.
			// Which is in reference to looking up gameObject by instanceID
			Scenes::Scene* scene = Scenes::SceneManager::getActiveScene();
			std::string instanceID = iterator->get<nlohmann::json>()["instanceID"];
			Core::GameObject* foundGameObject = scene->getGameObject(instanceID);

			EditorNode* node = new EditorNode(foundGameObject);
			node->load(iterator->get<nlohmann::json>());
			nodes.push_back(node);
		}
	} else
	{
		std::cout << "Did you try to load the wrong json file for the editor node tree?\m";
	}
}

nlohmann::json EditorNodeTree::getData()
{
	nlohmann::json output;
	for (auto node : nodes)
	{
		output.push_back(*node->getData());
	}
	return output;
}

EditorNode* EditorNodeTree::findNodeByInstanceID(std::string nodeInstanceID)
{
	for (int i = 0; i < nodes.size(); ++i)
	{
		//When correct instanceID is found
		if (nodes[i]->connectedGameObject->transform->getInstanceID() == nodeInstanceID)
			return nodes[i];
	}
	throw std::exception("InstanceID couldn't be found");
}

void EditorNodeTree::createParentalBonds()
{
	for (int i = 0; i < nodes.size(); ++i)
	{
		const auto parent = nodes[i]->connectedGameObject->transform->getParent();
		if (parent == nullptr) continue;
		const std::string nodeInstanceID = parent->getInstanceID();
		nodes[i]->move(findNodeByInstanceID(nodeInstanceID));
	}
}
