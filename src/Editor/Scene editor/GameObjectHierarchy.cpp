#ifdef TRISTEON_EDITOR

#include "GameObjectHierarchy.h"
#include "Editor/Asset Browser/AssetBrowser.h"
#include "Core/GameObject.h"
#include "Editor/EditorSelection.h"
#include <GLFW/glfw3.h>
#include "Scenes/SceneManager.h"
#include "Editor/EditorDragging.h"
#include "Editor/JsonSerializer.h"

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
	if (nodeData["typeID"] != Tristeon::Core::Type<Core::GameObject>::fullName())
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
	//Is the item hovered and is the mouse dragging?
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && ImGui::IsMouseClicked(0,false)) EditorDragging::setDragableItem(node);

	//Check if a dragging node is being dropped
	EditorNode* draggingNode = dynamic_cast<EditorNode*>(EditorDragging::getDragableItem());
	const bool isHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
	if (isHovered) hoveredNode = node;
	const bool isMouseReleased = ImGui::IsMouseReleased(0);
	bool hasChild = false;
	if (draggingNode != nullptr) hasChild = draggingNode->hasChild(node);

	//If dragging node is not null and the it is hovered above the last UI element it
	if (draggingNode != nullptr && draggingNode != node && !hasChild && isHovered && isMouseReleased)
	{
		draggingNode->move(node);
		EditorDragging::reset();
	}

	if (nodeOpen)
	{
		for (EditorNode* childNode : node->children)
			drawNode(childNode);
		ImGui::TreePop();
	}
}

void GameObjectHierarchy::loadScene(Tristeon::Scenes::Scene& scene)
{
	//Load all gameobjects to the gameobject hierarchy
	editorNodeTree.load(scene.serialize()["gameObjects"]);
	//Create parent relationships
	editorNodeTree.createParentalBonds();
}

void GameObjectHierarchy::onGui()
{
	checkSceneChanges();

	ImGui::Begin("Hierarchy", 0, windowFlags);

	//Right click popup for gameobject creation
	if (ImGui::BeginPopupContextWindow("Asset creation"))
	{
		//GUI text for object name
		ImGui::InputText("Gameobject name", createdGameObjectName, 255);
		if (ImGui::Button("Empty GameObject") || ImGui::IsKeyPressed(257, false))
		{
			createGameObject(createdGameObjectName);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::Button("Cube"))
		{
			//Create Cube
			Core::GameObject* gameObj = JsonSerializer::deserialize<Core::GameObject>("Files/Primitives/Cube.prefab");
			createGameObject(gameObj);
		}
		ImGui::SameLine();
		if (ImGui::Button("Sphere"))
		{
			//Create Sphere
			Core::GameObject* gameObj = JsonSerializer::deserialize<Core::GameObject>("Files/Primitives/Sphere.prefab");
			createGameObject(gameObj);
		}
		ImGui::SameLine();
		if (ImGui::Button("Plane"))
		{
			//Create Plane
			//TODO: add plane prefab
			//Core::GameObject* gameObj = JsonSerializer::deserialize<Core::GameObject>("Files\Primitives\Plane.prefab");
			//createGameObject(gameObj);
		}
		ImGui::EndPopup();
	}

	//Popup for renaming
	if (ImGui::BeginPopup("Edit Node Name"))
	{
		if (selectedNode != nullptr)
		{

		}
		else
		ImGui::EndPopup();
	}

	//Handle input
	handleInput();

	//Draw nodes
	for (int i = 0; i < editorNodeTree.nodes.size(); i++)
	{
		if (editorNodeTree.nodes[i]->parent == nullptr) drawNode(editorNodeTree.nodes[i].get());
	}


	hoveredNode = nullptr;

	//Reset
	ImGui::End();
}

void GameObjectHierarchy::checkSceneChanges()
{
	if (currentScene == nullptr || currentScene == NULL) currentScene = Scenes::SceneManager::getActiveScene();
	else if (currentScene != nullptr && currentScene != Scenes::SceneManager::getActiveScene())
	{
		loadScene(*Scenes::SceneManager::getActiveScene());
		currentScene = Scenes::SceneManager::getActiveScene();
	}
}

void GameObjectHierarchy::handleDroppedItems()
{
	EditorNode* draggingNode = dynamic_cast<EditorNode*>(EditorDragging::getDragableItem());
	//Drop dragged item outside of any gameobject, removing the parental bond
	if (ImGui::IsMouseReleased(0) && draggingNode != nullptr && hoveredNode == nullptr && draggingNode->parent != nullptr && ImGui::IsWindowHovered())
	{
		draggingNode->move(nullptr);
		EditorDragging::reset();
	}

	PrefabFileItem* prefabFile = dynamic_cast<PrefabFileItem*>(EditorDragging::getDragableItem());
	//Dopped prefab file without hovering over any nodes
	if (ImGui::IsMouseReleased(0) && draggingNode != nullptr && hoveredNode == nullptr && draggingNode->parent != nullptr && ImGui::IsWindowHovered())
	{
		Core::GameObject* gameObj = new Core::GameObject();
		gameObj->deserialize(prefabFile->GetPrefabData());
		createGameObject(gameObj);
		selectedNode->setPrefab(prefabFile->getFilePath());
	}
}

void GameObjectHierarchy::createGameObject(std::string name)
{
	std::unique_ptr<Core::GameObject> gameObject = std::make_unique<Core::GameObject>();

	//Load gameobject into an editorNode
	EditorNode* createdNode = new EditorNode(gameObject.get());
	gameObject->name = createdGameObjectName;
	createdNode->load(gameObject->serialize());

	//Add the new gameobject to the current scene
	Scenes::SceneManager::getActiveScene()->addGameObject(std::move(gameObject));

	//If a node has been selected use it as the new parent of the created gameobject
	if (selectedNode != nullptr)
	{
		createdNode->setParent(selectedNode);
	}
	editorNodeTree.nodes.push_back(std::move(std::unique_ptr<EditorNode>(createdNode)));
	selectedNode = createdNode;
	EditorSelection::setSelectedItem(createdNode);
}

void GameObjectHierarchy::createGameObject(Tristeon::Core::GameObject* gameObject)
{
	Scenes::SceneManager::getActiveScene()->addGameObject(std::move(std::unique_ptr<Core::GameObject>(gameObject)));

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
	editorNodeTree.nodes.push_back(std::move(std::unique_ptr<EditorNode>(createdNode)));
	selectedNode = createdNode;
	EditorSelection::setSelectedItem(createdNode);
}

void GameObjectHierarchy::handleInput()
{
	if (selectedNode != nullptr)
	{
		if (ImGui::IsKeyPressed(GLFW_KEY_DELETE, false))
		{
			editorNodeTree.removeNode(selectedNode);
			selectedNode = nullptr;
			EditorSelection::setSelectedItem(nullptr);
		}

		if (ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && ImGui::IsKeyPressed(GLFW_KEY_D, false))
		{
			auto gameObj = new Core::GameObject();
			gameObj->deserialize(*selectedNode->getData());
			createGameObject(gameObj);
		}

		if (ImGui::IsKeyPressed(GLFW_KEY_F2, false))
		{
			ImGui::OpenPopup("Edit Node Name");
		}
	}
}

void GameObjectHierarchy::changeNodeName()
{
	ImGui::OpenPopup("Edit Node Name");
}

#endif
