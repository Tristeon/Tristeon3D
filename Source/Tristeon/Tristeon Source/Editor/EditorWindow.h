#pragma once
#include "../../ImGUI/imgui.h"

namespace Tristeon
{
	namespace Editor
	{
		/**
		 * \brief EditorWindow is an interface to inherit from to create your own window, using imgui calls and overriding the ongui
		 * function which currently is called every frame. EditorWindows currently have to be manually registered to the Editor
		 */
		class EditorWindow
		{
			friend class TristeonEditor;
		public:
			EditorWindow();
			virtual ~EditorWindow();
			/**
			 * \brief onGUI is called for rendering and handling GUI events.
			 *		Currently onGUI is called per-frame however that might be subject to change
			 */
			virtual void onGui() {}

		protected:
			ImGuiWindowFlags windowFlags;
			static TristeonEditor* editor;
		};
	}
}
