#if TRISTEON_EDITOR

#include "SceneWindow.h"
#include "Editor/Asset Browser/AssetBrowser.h"
#include <Core/Rendering/Vulkan/HelperClasses/CameraRenderData.h>

//Gizmos
#include <ImGUI/Plugins/ImGuizmo.h>
#include "Core/Transform.h"
#include <glm/gtc/matrix_transform.inl>
#include "Misc/Hardware/Mouse.h"
#include "Editor/EditorSelection.h"
#include "Core/GameObject.h"
#include "Core/Components/Camera.h"
#include "Editor/EditorNode.h"

const int yOffset = 110;

Tristeon::Editor::SceneWindow::SceneWindow(TristeonEditor* ed) : editor(ed)
{
}

void Tristeon::Editor::SceneWindow::onGui()
{
	ImGui::Begin("Scene", 0, windowFlags | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	if (ImGui::IsWindowFocused())
	{
		//Move camera using input
		moveCamera();
	}

	//Set input for the gizmos settings
	setInputMode();

	//Draws the scene
	drawScene();

	//Draws gizmos over the scene
	EditorNode* obj = dynamic_cast<EditorNode*>(EditorSelection::getSelectedItem());
	
	//Is the selected editor item an EditorNode?
	if (obj != nullptr)
	{
		nlohmann::json* data = obj->getData();
		if ((*data)["typeID"] == typeid(Core::GameObject).name())
		{
			Core::GameObject temp;
			temp.deserialize(*data);

			glm::mat4 model = temp.transform->getTransformationMatrix();

			//Draw
			drawGizmos(model);

			//Get our modified matrix and apply to transform
			float t[3], r[3], s[3];
			ImGuizmo::DecomposeMatrixToComponents(&model[0][0], t, r, s);
			temp.transform->localPosition = Math::Vector3(t[0], t[1], t[2]);
			temp.transform->localRotation = Math::Quaternion::euler(r[0], r[1], r[2]);
			temp.transform->localScale = Math::Vector3(s[0], s[1], s[2]);

			//Save
			*data = temp.serialize();
			obj->applyChanges();
		}
	}

	ImGui::End();
}

void Tristeon::Editor::SceneWindow::moveCamera()
{
	if (!ImGui::IsWindowFocused())
		return;

	if (ImGuizmo::IsOver())
		return;

	cameraController.update(editor->editorCamera->trans);
	//bool dragging = Misc::Mouse::getButton(Misc::MouseButton::BUTTON_LEFT);
	//bool draggingRight = Misc::Mouse::getButton(Misc::MouseButton::BUTTON_RIGHT);

	//if (dragging)
	//	editor->editorCamera->trans->translate(Math::Vector3(Misc::Mouse::getMouseDelta().x * sens, -Misc::Mouse::getMouseDelta().y * sens, 0));
	//editor->editorCamera->trans->translate(Math::Vector3(0, 0, -Misc::Mouse::getScroll().y * sens));

	////if (draggingRight)
	//	//editor->editorCamera->trans->rotate(Math::Vector3(1, 0, 0), Misc::Mouse::getMouseDelta().y * sens);
	//if (draggingRight)
	//	editor->editorCamera->trans->rotate(Math::Vector3(0, 1, 0), Misc::Mouse::getMouseDelta().x * sens);
}

void Tristeon::Editor::SceneWindow::setInputMode()
{
	//Input mode
	if (ImGui::RadioButton("Translate", inputOperation == ImGuizmo::TRANSLATE) || ImGui::IsKeyDown(GLFW_KEY_W))
		inputOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", inputOperation == ImGuizmo::ROTATE) || ImGui::IsWindowFocused() && ImGui::IsKeyDown(GLFW_KEY_E))
		inputOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", inputOperation == ImGuizmo::SCALE) || ImGui::IsWindowFocused() && ImGui::IsKeyDown(GLFW_KEY_R))
		inputOperation = ImGuizmo::SCALE;

	//World/global
	if (ImGui::RadioButton("Local", transformationMode == ImGuizmo::LOCAL))
		transformationMode = ImGuizmo::LOCAL;
	ImGui::SameLine();
	if (ImGui::RadioButton("World", transformationMode == ImGuizmo::WORLD))
		transformationMode = ImGuizmo::WORLD;
}

void Tristeon::Editor::SceneWindow::drawScene() const
{
	Core::Rendering::Vulkan::CameraRenderData* data = editor->editorCamera->cam;
	ImVec2 size = ImGui::GetWindowSize();
	size.y -= yOffset;
	editor->editorCamera->size = Math::Vector2(size.x, size.y);
	if (data != nullptr)
		ImGui::Image((ImTextureID)(VkDescriptorSet)data->onscreen.sets[0], size);
}

void Tristeon::Editor::SceneWindow::drawGizmos(glm::mat4& model) const
{
	ImVec2 size = ImGui::GetWindowSize();
	size.y -= yOffset;

	glm::mat4 view = Core::Components::Camera::getViewMatrix(editor->editorCamera->trans);
	glm::mat4 proj = Core::Components::Camera::getProjectionMatrix(size.x / size.y, 60, 0.1f, 1000.0f);

	ImVec2 const rectSize = ImGui::GetItemRectSize();
	ImVec2 const rectMin = ImGui::GetItemRectMin();

	ImGuizmo::SetRect(rectMin.x, rectMin.y, rectSize.x, rectSize.y);

	ImGuizmo::SetDrawlist();
	ImGuizmo::Manipulate(&view[0][0], &proj[0][0], inputOperation, transformationMode, &model[0][0], NULL, NULL);
}

#endif