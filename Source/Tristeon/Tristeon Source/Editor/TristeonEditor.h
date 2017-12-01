#pragma once
#include <vector>
#include "EditorWindow.h"
#include "Core/BindingData.h"
#include "Core/Rendering/RenderManager.h"
#include "Core/Rendering/Vulkan/RenderManagerVulkan.h"

namespace Tristeon
{
	namespace Core {
		class Message;
		class Engine;
	}

	namespace Editor
	{
		class SceneWindow;

		class TristeonEditor
		{
			friend SceneWindow;

		public:
			TristeonEditor();
			~TristeonEditor();
			/**
			 * \brief Sets the style of the editor. colors, spacing, etc.
			 */
			void setStyle();
			/**
			 * \brief Initialization of the editor needs to be called after the instance is created so it can bind to the engine.
			 */
			void init(Core::Engine* bindingData);
			/**
			 * \brief OnGui calls the imgui calls and deals with all editor behaviors. might be subject to change
			 */
			void onGui();
			/**
			 * \brief Calls all the internal render calls necessary to run the editor
			 */
			void render();
		private:
			/**
			 * \brief Binds imgui to the rendering implementation
			 */
			void bindImGui(Core::VulkanBindingData* vkBinding);
			/**
			 * \brief Creates the font textures and loads them
			 */
			void initFontsImGui(Core::VulkanBindingData* vkBinding);
			/**
			 * \brief Subscribes the editor to all generic callbacks received from the engine through the ManagerProtocol
			 */
			void setupCallbacks();
			/**
			 * \brief Creates vulkan command buffers
			 */
			void createCommandBuffers();

			//Editor
			/**
			 * \brief The windows the editor displays using the ongui calls
			 */
			std::vector<EditorWindow*> windows;

			//Rendering
			Core::Rendering::UIRenderable* renderable = nullptr;
			std::vector<vk::CommandBuffer> cmdBuffers;
			vk::Device vkDevice;

			//Ref
			Core::Engine* engine = nullptr;

			Core::Rendering::Vulkan::EditorData* editorCamera;

			bool inPlayMode = false;
		};
	}
}