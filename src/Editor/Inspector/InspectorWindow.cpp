#ifdef TRISTEON_EDITOR

#include "InspectorWindow.h"
#include <ImGUI/imgui.h>
#include "Editor/EditorSelection.h"
#include "Editor/EditorNode.h"
#include "Editor/Selectable.h"
#include "Editor/Asset Browser/AssetItem.h"
#include "Core/Components/Camera.h"
#include <Core/Rendering/Components/MeshRenderer.h>
#include "Misc/StringUtils.h"
#include <string.h>
using namespace Tristeon::Editor;

InspectorWindow::InspectorWindow()
{
}

void InspectorWindow::onGui()
{
	ImGui::Begin("Inspector window", 0, windowFlags);

	Selectable* selectedItem = EditorSelection::getSelectedItem();
	if (selectedItem != nullptr)
	{
		if (dynamic_cast<EditorNode*>(selectedItem) != nullptr)
		{
			//Display selected editornode
			drawEditorNode((EditorNode*)selectedItem);
		}
		else if (dynamic_cast<AssetItem*>(selectedItem) != nullptr)
		{
			if (dynamic_cast<PrefabFileItem*>(selectedItem) != nullptr)
			{
				drawPrefab((PrefabFileItem*)selectedItem);
			}
			//Display assetitem
			else dynamic_cast<AssetItem*>(selectedItem)->drawOnInspector();
		}
	}
	ImGui::End();
}

void InspectorWindow::drawEditorNode(EditorNode* node)
{
	nlohmann::json* data = node->getData();

	//TODO: Create function for string fields, bool,etc.

	//Load main gameobject inspector UI
	//GameObject active
	bool active = (*data)["active"];
	ImGui::Checkbox("", &active);
	(*data)["active"] = active;

	ImGui::SameLine();

	//GameObject name
	std::string gameObjectName = (*data)["name"];
	char nameValue[255] = "";
	strcpy_s(nameValue, 255, gameObjectName.c_str());

	//Draw text field without label
	ImGui::PushItemWidth(-1);
	ImGui::InputText("##label", nameValue,255);
	ImGui::PopItemWidth();

	//Apply editor changes
	(*data)["name"] = nameValue;

	//Display prefab interface if the node is a prefab
	if (node->isPrefab())
	{
		if (ImGui::Button("Apply Prefab"))
		{
			node->setPrefabData(*data);
		}
		ImGui::SameLine();
		if (ImGui::Button("Revert Prefab"))
		{
			*data = node->getPrefabData();
		}
	}

	drawGameObjectContent(*data);

	node->applyChanges();
}

void InspectorWindow::drawPrefab(PrefabFileItem* prefabData)
{
	nlohmann::json data = prefabData->GetPrefabData();

	//TODO: Create function for string fields, bool,etc.

	//Load main gameobject inspector UI
	//GameObject active
	bool active = data["active"];
	ImGui::Checkbox("", &active);
	data["active"] = active;

	ImGui::SameLine();

	//GameObject name
	std::string gameObjectName = data["name"];
	char nameValue[255] = "";
	strcpy_s(nameValue, 255, gameObjectName.c_str());

	//Draw text field without label
	ImGui::PushItemWidth(-1);
	ImGui::InputText("##label", nameValue, 255);
	ImGui::PopItemWidth();

	//Apply editor changes
	data["name"] = nameValue;

	drawGameObjectContent(data);

	prefabData->SetPrefabData(data);
}

void InspectorWindow::drawGameObjectContent(nlohmann::json& data)
{
	//Display transform
	if (ImGui::CollapsingHeader("Transform"))
	{

		//Get transform data
		nlohmann::json transformData = data["transform"];
		nlohmann::json positionData = transformData["localPosition"];
		nlohmann::json rotationData = transformData["localRotation"];
		nlohmann::json scaleData = transformData["localScale"];

		//Reset button
		if (ImGui::Button("Reset"))
		{
			positionData["x"] = 0;
			positionData["y"] = 0;
			positionData["z"] = 0;
			rotationData["x"] = 0;
			rotationData["y"] = 0;
			rotationData["z"] = 0;
			scaleData["x"] = 1;
			scaleData["y"] = 1;
			scaleData["z"] = 1;
		}

		//Set position data
		float position[3];
		position[0] = positionData["x"];
		position[1] = positionData["y"];
		position[2] = positionData["z"];
		ImGui::DragFloat3("Position", position);

		//Get and apply changed position
		positionData["x"] = position[0];
		positionData["y"] = position[1];
		positionData["z"] = position[2];

		//Set rotation data
		float rotation[3];
		rotation[0] = rotationData["x"];
		rotation[1] = rotationData["y"];
		rotation[2] = rotationData["z"];
		ImGui::DragFloat3("Rotation", rotation);

		//Get and apply changed rotation
		rotationData["x"] = rotation[0];
		rotationData["y"] = rotation[1];
		rotationData["z"] = rotation[2];


		//Set scale data
		float scale[3];
		scale[0] = scaleData["x"];
		scale[1] = scaleData["y"];
		scale[2] = scaleData["z"];
		ImGui::DragFloat3("Scale", scale);

		//Get and apply changed scale
		scaleData["x"] = scale[0];
		scaleData["y"] = scale[1];
		scaleData["z"] = scale[2];

		//Apply all changes
		transformData["localPosition"] = positionData;
		transformData["localRotation"] = rotationData;
		transformData["localScale"] = scaleData;
		data["transform"] = transformData;
	}

	//If there are more components than closeableHeaders increment the array.
	if (data["components"].size() >= closeableHeaders.size())
		closeableHeaders.push_back(true);

	int indexToErase = -1;
	nlohmann::json componentsData = data["components"];

	//Display components
	for (int i = 0; i < componentsData.size(); i++)
	{
		std::string nodeLabel = typeIdNameToClassName(componentsData.at(i)["typeID"]);
		bool closeableHeader = closeableHeaders[i];
		if (ImGui::CollapsingHeader(nodeLabel.c_str(), &closeableHeader))
		{
			drawSerializedObject(componentsData.at(i));
		}
		closeableHeaders[i] = closeableHeader;

		if (closeableHeaders[i] == false)
			indexToErase = i;
	}
	data["components"] = componentsData;

	if (indexToErase != -1)
	{
		data["components"].erase(indexToErase);
		closeableHeaders[closeableHeaders.size() - 1] = true;
	}

	//TODO: remove hard-code and implement non-hard-code solution
	if (ImGui::Button("Add component"))
		ImGui::OpenPopup("select component");

	std::vector<char*> components = { "Camera", "MeshRenderer" };

	if (ImGui::BeginPopup("select component"))
	{
		for (int i = 0; i < components.size(); i++)
		{
			//When component is selected
			if (ImGui::Selectable(components[i]))
			{
				//TODO: remove hard-code
				//Create selected component
				if (i == 0)
				{
					data["components"].push_back(Core::Components::Camera().serialize());
				}
				else if (i == 1)
				{
					data["components"].push_back(Core::Rendering::MeshRenderer().serialize());
				}
			}
		}
		ImGui::EndPopup();
	}
}

void InspectorWindow::drawSerializedObject(nlohmann::json& serializedComponent)
{
	for (auto iterator = serializedComponent.begin(); iterator != serializedComponent.end(); ++iterator)
	{
		//TODO: cleanup hard-code
		if (iterator.key() == "typeID" || iterator.key() == "instanceID" || iterator.key() == "name") continue;
		switch (iterator.value().type())
		{
		case nlohmann::detail::value_t::boolean:
		{
			bool boolValue = serializedComponent[iterator.key()];
			ImGui::Checkbox(iterator.key().c_str(), &boolValue);
			serializedComponent[iterator.key()] = boolValue;
			break;
		}
		case nlohmann::detail::value_t::string:
		{
			std::string stringValue = serializedComponent[iterator.key()];
			char charValue[255] = "Wut";
			strcpy_s(charValue, 255, stringValue.c_str());
			ImGui::InputText(iterator.key().c_str(), charValue, 255,ImGuiInputTextFlags_EnterReturnsTrue);
				serializedComponent[iterator.key()] = charValue;
			break;
		}
		case nlohmann::detail::value_t::object:
			drawSerializedObject(serializedComponent[iterator.key()]);
			break;
		case nlohmann::detail::value_t::array:
			break;
		case nlohmann::detail::value_t::number_float:
		{
			float floatValue = serializedComponent[iterator.key()];
			ImGui::InputFloat(iterator.key().c_str(), &floatValue);
			serializedComponent[iterator.key()] = floatValue;
			break;
		}
		case nlohmann::detail::value_t::number_integer:
		case nlohmann::detail::value_t::number_unsigned:
		{
			int intValue = serializedComponent[iterator.key()];
			ImGui::InputInt(iterator.key().c_str(), &intValue);
			serializedComponent[iterator.key()] = intValue;
			break;
		}
		default:
			break;
		}
	}
}

std::string InspectorWindow::typeIdNameToClassName(std::string typeIdName)
{
	auto splitTypeIdName = StringUtils::split(typeIdName, '::');

	//If the type is not within a namespace it will not use any ::
	if (splitTypeIdName.size() == 1) //in the case of "class className" split the space and take the last item which is the class name
		return StringUtils::split(splitTypeIdName[0], ' ')[1];
	else //in the case of "class namespace::classname" simply get the last member of the '::' split
		return splitTypeIdName.back();
}

#endif