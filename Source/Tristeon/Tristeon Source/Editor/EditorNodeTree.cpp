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
			Scenes::Scene* scene = Scenes::SceneManager::getActiveScene();
			int instanceID = iterator->get<nlohmann::json>()["instanceID"];
			std::shared_ptr<Tristeon::Core::GameObject> foundGameObject;
			for (std::shared_ptr<Core::GameObject> gameObject : scene->gameObjects)
			{
				if (gameObject->getInstanceID() == instanceID) foundGameObject = gameObject;
			}

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