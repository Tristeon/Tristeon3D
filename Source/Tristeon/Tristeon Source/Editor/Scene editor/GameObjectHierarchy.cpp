#include "GameObjectHierarchy.h"
#include "Editor/Asset Browser/AssetBrowser.h"
#include "Core/GameObject.h"
#include "Editor/EditorSelection.h"
#include <GLFW/glfw3.h>
#include "Scenes/SceneManager.h"

using namespace Tristeon::Editor;

GameObjectHierarchy::GameObjectHierarchy()
{
}


GameObjectHierarchy::~GameObjectHierarchy()
{
}

void GameObjectHierarchy::drawNode(EditorNode* node)
{
	//Retrieve data
	nlohmann::json nodeData = *node->getData();

	//Type check
	if (nodeData["typeID"] != typeid(Core::GameObject).name())
	{
		std::cout << "Editor node for the GameObject hierarchy is not a GameObject, b-b-baka~!\n";
		return;
	}

	//Node flag conditions
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	if (node->children.size() == 0) flags |= ImGuiTreeNodeFlags_Leaf;
	if (node == selectedNode)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	//Load gui elements
	const std::string nodeName = nodeData["name"];
	const bool nodeOpen = ImGui::TreeNodeEx(nodeName.c_str(), flags);

	//Check if the user pressed the window and we need to unselect the curretly selected node
	if (ImGui::IsMouseClicked(0) && selectedNode == node && ImGui::IsWindowHovered())
	{
		selectedNode = nullptr;
	}

	//If the ui item is pressed select it
	if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1))
	{
		std::cout << "Node: " << nodeData["name"] << " was selected\n";
		EditorSelection::setSelectedItem(node);
		selectedNode = node;
	}
	//Is the item active and is the mouse dragging?
	if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) draggingNode = node;

	//Dropped dragging node
	bool isHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
	bool isMouseReleased = ImGui::IsMouseReleased(0);
	bool hasChild = false;
	if (draggingNode != nullptr) hasChild = draggingNode->hasChild(node);
	if (draggingNode != nullptr && draggingNode != node && !hasChild && isHovered && isMouseReleased)
	{
		if (draggingNode->parent == nullptr)
		{
			//Remove direct relation to the editor tree
			auto removeIterator = remove(editorNodeTree.nodes.begin(), editorNodeTree.nodes.end(), draggingNode);
			editorNodeTree.nodes.erase(removeIterator);
		}
		draggingNode->move(node);
		draggingNode = nullptr;
	}

	if (nodeOpen)
	{
		for (EditorNode* childNode : node->children)
			drawNode(childNode);
		ImGui::TreePop();
	}
}

void GameObjectHierarchy::loadScene(Tristeon::Scenes::Scene scene)
{
	editorNodeTree.load(scene.serialize()["gameObjects"]);
}

void GameObjectHierarchy::onGui()
{
	checkSceneChanges();

	ImGui::Begin("Hierarchy", 0, windowFlags);

	//Temporary Test code
	if (ImGui::IsKeyPressed(GLFW_KEY_T, false) && ImGui::IsWindowHovered())
	{
		std::shared_ptr<Core::GameObject> gameObject = std::make_shared<Core::GameObject>();
		Scenes::SceneManager::getActiveScene()->gameObjects.push_back(gameObject);
		EditorNode* createdNode = new EditorNode(gameObject);
		gameObject->name = "Test";
		gameObject->tag = "Just a tag";
		createdNode->load(gameObject->serialize());
		if (selectedNode != nullptr &&
			(*selectedNode->getData())["typeID"] == typeid(Core::GameObject).name())
		{
			createdNode->parent = selectedNode;
			selectedNode->children.push_back(createdNode);
		}
		else editorNodeTree.nodes.push_back(createdNode);
		selectedNode = createdNode;
		EditorSelection::setSelectedItem(createdNode);
		ImGui::CloseCurrentPopup();
	}

	//Right click popup for gameobject creation
	if (ImGui::BeginPopupContextWindow("Asset creation"))
	{
		//GUI text for object name
		ImGui::InputText("Gameobject name", createdGameObjectName, 255);
		if (ImGui::Button("Create gameobject") || ImGui::IsKeyPressed(257, false))
		{
			//Add a new gameobject to the current scene
			std::shared_ptr<Core::GameObject> gameObject = std::make_shared<Core::GameObject>();
			Scenes::SceneManager::getActiveScene()->gameObjects.push_back(gameObject);

			//Load gameobject into an editorNode
			EditorNode* createdNode = new EditorNode(gameObject);
			gameObject->name = createdGameObjectName;
			createdNode->load(gameObject->serialize());

			//If a node has been selected use it as the new parent of the created gameobject
			if (selectedNode != nullptr)
			{
				createdNode->parent = selectedNode;
				selectedNode->children.push_back(createdNode);
			}
			else editorNodeTree.nodes.push_back(createdNode);
			selectedNode = createdNode;
			EditorSelection::setSelectedItem(createdNode);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	//TODO: use global dragging for this
	//Reset drag item
	if (!ImGui::IsMouseReleased(0) && !ImGui::IsMouseDragging(0)) draggingNode = nullptr; //don't reset on mouse release because that's when the drop is checked

	//Draw nodes
	for (int i = 0; i < editorNodeTree.nodes.size(); i++)
	{
		drawNode(editorNodeTree.nodes[i]);
	}

	if (draggingNode != nullptr) std::cout << "Dragging node: " << (*draggingNode->getData())["name"] << std::endl;

	//Drop dragged item outside of any gameobject, removing the parental bond
	if (ImGui::IsMouseReleased(0) && draggingNode != nullptr && draggingNode->parent != nullptr && ImGui::IsWindowHovered())
	{
		draggingNode->move(nullptr);
		editorNodeTree.nodes.push_back(draggingNode);
		draggingNode = nullptr;
	}

	ImGui::End();
}

void GameObjectHierarchy::checkSceneChanges()
{
	if (currentScene == nullptr || currentScene == NULL) currentScene = Scenes::SceneManager::getActiveScene();
	else if (currentScene != Scenes::SceneManager::getActiveScene())
	{
		loadScene(*Scenes::SceneManager::getActiveScene());
		currentScene = Scenes::SceneManager::getActiveScene();
	}
}
