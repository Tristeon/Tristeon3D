#pragma once
#include "Editor/EditorWindow.h"
#include "Editor/TristeonEditor.h"
#include "../../../ImGUI/Plugins/ImGuizmo.h"
#include "EditorCameraController.h"

namespace Tristeon
{
	namespace Editor
	{
		class SceneWindow : public EditorWindow
		{
		public:
			explicit SceneWindow(TristeonEditor* ed);
			/**
			* \brief onGUI is called for rendering and handling GUI events.
			*		Currently onGUI is called per-frame however that might be subject to change
			*/
			void onGui() override;

		private:
			void moveCamera();
			/**
			 * \brief Set input for the gizmos settings
			 */
			void setInputMode();
			/**
			 * \brief Render the scene
			 */
			void drawScene() const;
			void drawGizmos(glm::mat4& model) const;

			EditorCameraController cameraController;

			TristeonEditor* editor;

			ImGuizmo::OPERATION inputOperation = ImGuizmo::ROTATE;
			ImGuizmo::MODE transformationMode = ImGuizmo::LOCAL;

			const float sens = 0.1f;
		};
	}
}
