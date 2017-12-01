#pragma once
#include "Editor/EditorWindow.h"
#include <vector>
#include "Editor/json.hpp"

namespace Tristeon
{
	namespace Editor
	{
		class EditorNode;

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
			/**
			 * \brief Modularly draws a component of a gameobject using the given json information
			 */
			void drawComponent(nlohmann::json& serializedComponent);

			/**
			 * \brief typeid(T).name() is used to store typeids, in order to retrieve the name of a type the function
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
