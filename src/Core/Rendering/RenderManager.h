#pragma once
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

				void buildSwapchain();
				void cleanSwapchain();
				
				bool inPlayMode = false;
				bool resized = false;
				/**
				 * \brief The only instance of RenderManager ever. Used so that getMaterial() can access local variables
				 */
				static RenderManager* instance;
				std::unique_ptr<Material> screenMat;
				
				static FrameBufferAttachment createAttachment(vk::Format format, vk::ImageAspectFlags aspect, vk::ImageLayout layout);
			};
		}
	}
}

#if !defined(TRISTEON_LOGENABLED)
#define VULKAN_DEBUG function;
#else
#define VULKAN_DEBUG(function) { \
		const vk::Result result = function; \
		if (result != vk::Result::eSuccess) \
			Tristeon::Misc::Console::write("[VULKAN] [ERROR] [CODE: " + vk::to_string(result) + "] " + #function); \
		}
#endif