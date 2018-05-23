#pragma once
#ifdef TRISTEON_EDITOR

#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Math/Quaternion.h"
#include "Core/Transform.h"

namespace Tristeon
{
	namespace Core {
		class Transform;
	}
	namespace Editor
	{
		class EditorCameraController
		{
		public:
			EditorCameraController() = default;
			void update(Tristeon::Core::Transform* editorCamera);
		private:
			void setPosition(Tristeon::Core::Transform* editorCamera);
			void rotateAroundFocus(Tristeon::Core::Transform* editorCamera);
			void moveCamera(Core::Transform* editorCamera);
			Core::Transform focusPoint;
			float distanceFromTarget = 5;
			float rotationSpeed = 0.5f;
			float dragSpeed = 0.1f;
			float zoomSpeed = 0.1f;
			float xRotation = 0;
			float yRotation = 0;
		};
	}
}

#endif