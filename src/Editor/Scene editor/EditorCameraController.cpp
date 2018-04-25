#if TRISTEON_EDITOR

#include "EditorCameraController.h"
#include "Core/Transform.h"
#include "Misc/Hardware/Keyboard.h"
#include "Misc/Hardware/Mouse.h"
#include "Editor/EditorNode.h"
#include "Editor/EditorSelection.h"
#include "GameObjectHierarchy.h"

using namespace Tristeon::Editor;

void EditorCameraController::update(Tristeon::Core::Transform* editorCamera)
{
	//xRotation += 0.08f;
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

	if (Misc::Keyboard::getKey(Misc::KeyCode::LEFT_ALT))
	{
		if (Misc::Keyboard::getKey(Misc::KeyCode::LEFT_CONTROL) && Misc::Mouse::getButton(Misc::MouseButton::BUTTON_LEFT))
			moveCamera(editorCamera);
		else if (Misc::Mouse::getButton(Misc::MouseButton::BUTTON_LEFT))
			rotateAroundFocus(editorCamera); //Rotate around the focus point
		else if (Misc::Mouse::getButton(Misc::MouseButton::BUTTON_MIDDLE))
			moveCamera(editorCamera);
		else if (Misc::Mouse::getButton(Misc::MouseButton::BUTTON_RIGHT))
			distanceFromTarget -= Misc::Mouse::getMouseDelta().x;
	}
	setPosition(editorCamera);
}

void EditorCameraController::setPosition(Tristeon::Core::Transform* editorCamera)
{
	//editorCamera->position = focusPoint.position + -distanceFromTarget * focusPoint.forward();
	editorCamera->position = focusPoint.position + Math::Vector3::back * distanceFromTarget * Math::Quaternion::euler(xRotation,yRotation,0);
	editorCamera->lookAt(&focusPoint);
}

void EditorCameraController::rotateAroundFocus(Tristeon::Core::Transform* editorCamera)
{
	Math::Vector2 mouseDelta = Misc::Mouse::getMouseDelta();
	//focusPoint.rotate(Math::Vector3::up, rotationSpeed *mouseDelta.x);
	//focusPoint.rotate(Math::Vector3::right, rotationSpeed *mouseDelta.y);
	yRotation -= rotationSpeed * mouseDelta.x;
	xRotation -= rotationSpeed * mouseDelta.y;
}

void EditorCameraController::moveCamera(Tristeon::Core::Transform* editorCamera)
{
	Math::Vector3 move;
	move += editorCamera->up() * dragSpeed * Misc::Mouse::getMouseDelta().y;
	move += editorCamera->right() * dragSpeed * -Misc::Mouse::getMouseDelta().x;
	focusPoint.translate(move);
}

#endif