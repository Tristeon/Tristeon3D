#pragma once
#ifdef TRISTEON_EDITOR
#include <vulkan/vulkan.hpp>
#include <vector>
#include <Editor/EditorWindow.h>

namespace Tristeon::Editor
{
	class TristeonEditor
	{
		friend class SceneWindow;
	public:
		TristeonEditor();
		~TristeonEditor();
		
		/**
		 * \brief Sets the style of the editor. colors, spacing, etc.
		 */
		void setStyle();
		/**
		 * \brief OnGui calls the imgui calls and deals with all editor behaviors. might be subject to change
		 */
		void onGui();
		/**
		 * \brief Calls all the internal render calls necessary to run the editor
		 */
		void render(vk::CommandBuffer cmd);

	private:
		/**
		 * \brief Binds imgui to the rendering implementation
		 */
		void bindImGui();
		/**
		 * \brief Creates the font textures and loads them
		 */
		void initFontsImGui();
		/**
		 * \brief Subscribes the editor to callbacks
		 */
		void setupCallbacks();

		//Editor
		/**
		 * \brief The windows the editor displays using the ongui calls
		 */
		std::vector<std::unique_ptr<EditorWindow>> windows;
		bool inPlayMode = false;
	};
}
#endif