#ifdef TRISTEON_EDITOR

#include "EditorNodeTree.h"
#include "Core/SceneManager.h"

using namespace Tristeon::Editor;

void EditorNodeTree::load(nlohmann::json nodeTree)
{
	nodes.clear();
	if (nodeTree.is_array())
	{
		for (auto iterator = nodeTree.begin(); iterator != nodeTree.end(); ++iterator)
		{
			//TODO: implement a faster implementation for loading scenes, this could be very performant intensive in big scenes.
			// Which is in reference to looking up gameObject by instanceID
			Core::Scene* scene = Core::SceneManager::current();
			std::string instanceID = iterator->get<nlohmann::json>()["instanceID"];
			Core::GameObject* foundGameObject = scene->get(instanceID);

			EditorNode* node = new EditorNode(foundGameObject);
			node->load(iterator->get<nlohmann::json>());
			if (!foundGameObject->prefabFilePath.empty())
			{
				node->setPrefab(foundGameObject->prefabFilePath);
			}
			nodes.push_back(std::move(std::unique_ptr<EditorNode>(node)));
		}
	} else
	{
		std::cout << "Did you try to load the wrong json file for the editor node tree?\n";
	}
}

nlohmann::json EditorNodeTree::getData()
{
	nlohmann::json output;
	for (int i = 0; i < nodes.size(); i++)
	{
		output.push_back(*nodes[i]->getData());
	}
	return output;
}

EditorNode* EditorNodeTree::findNodeByInstanceID(std::string nodeInstanceID)
{
	for (int i = 0; i < nodes.size(); ++i)
	{
		//When correct instanceID is found
		if (nodes[i]->connectedGameObject->transform.get()->getInstanceID() == nodeInstanceID)
			return nodes[i].get();
	}
	throw std::runtime_error("InstanceID couldn't be found");
}

void EditorNodeTree::createParentalBonds()
{
	for (int i = 0; i < nodes.size(); ++i)
	{
		const auto parent = nodes[i]->connectedGameObject->transform.get()->getParent();
		if (parent == nullptr) continue;
		const std::string nodeInstanceID = parent->getInstanceID();
		nodes[i]->move(findNodeByInstanceID(nodeInstanceID));
	}
}

void EditorNodeTree::removeNode(EditorNode* node)
{
	//Remove gameObject
	Core::SceneManager::current()->remove(node->connectedGameObject);

	//Remove node from nodetree
	for (int i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i].get() == node)
		{
			nodes.erase(nodes.begin() + i);
			return;
		}
	}
}

#endif
