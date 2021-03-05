#pragma once
#include <set>
#include <Core/Rendering/Material.h>
#include "Core/BindingData.h"

namespace Tristeon
{
	namespace Core
	{
		class Message;
		class TObject;
		
		namespace Rendering
		{
			class Renderer;
			
			/**
			 * \brief RenderManager renders the current scene.
			 */
			class RenderManager
			{
			public:
				RenderManager();
				~RenderManager();
			private:
				void setup();
				void render();

				static std::vector<const char*> getInstanceExtensions();
				static void createInstance(const std::vector<const char*>& extensions);
				static void createDebugMessenger();

				static void createSurface();
				static void selectPhysicalDevice();
				static std::set<uint32_t> selectQueueFamilies();
				static void createVirtualDevice(const std::set<uint32_t>& families);

				static void createDescriptorPool();
				static void createCommandPools();

				void createOnscreenMaterial();
				static void createOnscreenSemaphores();

				static void createOffscreenSemaphores();
				static void createOffscreenTransformLayout();
				static void allocateOffscreenCommandBuffer();

				void buildSwapchain() const;
				static void cleanSwapchain();

				static void createOnscreenImageViews();
				static void createOnscreenRenderpass();
				static void createOnscreenFramebuffers();

				static void createOffscreenRenderPass();
				static void createOffscreenFramebuffer();

				static void prepareMaterials();
				void recordOnscreenCommandBuffers() const;
				
				bool inPlayMode = false;
				bool resized = false;
				/**
				 * \brief The only instance of RenderManager ever. Used so that getMaterial() can access local variables
				 */
				static RenderManager* instance;
				std::unique_ptr<Material> screenMat;
				
				static FrameBufferAttachment createAttachment(vk::Format format, vk::ImageAspectFlags aspect, vk::ImageUsageFlags usage);
			};
		}
	}
}

#if !defined(TRISTEON_LOGENABLED)
#define VULKAN_DEBUG(function) function;
#else
#define VULKAN_DEBUG(function) { \
		const vk::Result result = function; \
		if (result != vk::Result::eSuccess) \
			Tristeon::Misc::Console::write("[VULKAN] [ERROR] [CODE: " + vk::to_string(result) + "] " + #function); \
		}
#endif