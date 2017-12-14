#include "EditorCameraController.h"
#include "Core/Transform.h"
#include "Misc/Hardware/Keyboard.h"
#include "Misc/Hardware/Mouse.h"
#include "Editor/EditorNode.h"
#include "Editor/EditorSelection.h"

using namespace Tristeon::Editor;

void EditorCameraController::update(Tristeon::Core::Transform* editorCamera)
{
	setPosition(editorCamera);

	distanceFromTarget -= Misc::Mouse::getScroll().y;
	distanceFromTarget = glm::clamp(distanceFromTarget, 1.f, 800.f);

	if (Misc::Keyboard::getKey(Misc::KeyCode::F))
	{
		if (dynamic_cast<EditorNode*>(EditorSelection::getSelectedItem()) != nullptr)
		{
			auto node = dynamic_cast<EditorNode*>(EditorSelection::getSelectedItem());
			focusPoint.position = node->connectedGameObject->transform->position;
		}
	}

	if (!Misc::Keyboard::getKey(Misc::KeyCode::LEFT_ALT))
		return;

	//Rotate around the focus point
	if (Misc::Mouse::getButton(Misc::MouseButton::BUTTON_LEFT))
		rotateAroundFocus(editorCamera);
	if (Misc::Mouse::getButton(Misc::MouseButton::BUTTON_MIDDLE))
		moveCamera(editorCamera);

}

void EditorCameraController::setPosition(Tristeon::Core::Transform* editorCamera)
{
	editorCamera->position = focusPoint.position + -distanceFromTarget * focusPoint.forward();
}

void EditorCameraController::rotateAroundFocus(Tristeon::Core::Transform* editorCamera)
{
	Math::Vector2 mouseDelta = Misc::Mouse::getMouseDelta();
	focusPoint.rotate(Math::Vector3::up, rotationSpeed *mouseDelta.x);
	focusPoint.rotate(Math::Vector3::right, rotationSpeed * mouseDelta.y);
	editorCamera->lookAt(&focusPoint);
}

void EditorCameraController::moveCamera(Tristeon::Core::Transform* editorCamera)
{
	Math::Vector3 move;
	move += focusPoint.up() * dragSpeed * Misc::Mouse::getMouseDelta().y;
	move += focusPoint.right() * dragSpeed * Misc::Mouse::getMouseDelta().x;
	focusPoint.translate(move);
}