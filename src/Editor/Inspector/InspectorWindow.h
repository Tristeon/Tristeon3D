#pragma once
#ifdef TRISTEON_EDITOR

#include "Editor/EditorWindow.h"
#include <vector>
#include "Editor/json.hpp"
#include "Editor/EditorNode.h"

class PrefabFileItem;

namespace Tristeon
{
	namespace Editor
	{
		class InspectorWindow : public EditorWindow
		{
		public:
			InspectorWindow();
			~InspectorWindow() override = default;
			/**
			* \brief onGUI is called for rendering and handling GUI events.
			* Currently onGUI is called per-frame however that might be subject to change
			*/
			void onGui() override;
		private:
			/**
			 * \brief Draws a gameobject to the inspector
			 */
			void drawEditorNode(EditorNode* node);

			void drawPrefab(PrefabFileItem* prefabData);

			/**
			 * \brief Draws the contents of the serialized gameobject
			 * which in this context is only the transform and the object's components
			 */
			void drawGameObjectContent(nlohmann::json& serializedGameObject);

			/**
			 * \brief Modularly draws a component of a gameobject using the given json information
			 */
			void drawSerializedObject(nlohmann::json& serializedComponent);

			/**
			 * \brief TRISTEON_TYPENAME is used to store typeids, in order to retrieve the name of a type the function
			 * splits the string until it only has the pure name without namespace and other strings.
			 */
			std::string typeIdNameToClassName(std::string typeIdName);

			/**
			 * \brief Which headers for the components are collapsed or open
			 */
			std::vector<bool> closeableHeaders;
		};
	}
}

#endif
