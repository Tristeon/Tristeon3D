#pragma once
#include "Core/Rendering/API/WindowContext.h"
#include <vulkan/vulkan.hpp>
#include "Core/Rendering/Vulkan/API/Extensions/DebugReportCallbackEXT.h"
#include <Core/Rendering/Vulkan/HelperClasses/Swapchain.h>
namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				class Swapchain;
				class Window;

				class WindowContextVulkan : public WindowContext
				{
				public:
					explicit WindowContextVulkan();
					~WindowContextVulkan();
					void prepareFrame() override;
					void finishFrame() override;

					vk::Instance getInstance() const { return instance; }
					vk::SurfaceKHR getSurfaceKHR() const { return surface; }
					vk::PhysicalDevice getGPU() const { return gpu; }
					vk::Device getDevice() const { return device; }
					vk::Queue getPresentQueue() const { return presentQueue; }
					vk::Queue getGraphicsQueue() const { return graphicsQueue; }
					vk::SwapchainKHR getSwapchainKHR() const { return swapchain->getSwapchain(); }
					Swapchain* getSwapchain() const { return swapchain.get(); }
					size_t getFramebufferCount() const { return swapchain->getFramebufferCount(); }
					vk::Extent2D getExtent() const { return swapchain->extent2D.get(); }
					vk::RenderPass getRenderpass() const { return swapchain->renderpass.get(); }
					vk::Semaphore getImageAvailable() const { return imageAvailable; }
					vk::Semaphore getRenderFinished() const { return renderFinished; }
					vk::Framebuffer getActiveFramebuffer() const { return swapchain->getFramebufferAt(imgIndex); }

				protected:
					void resize(int width, int height) override;

				private:
					void initInstance();
					void initDebugCallback();
					void initWindowSurface();
					void initGPU();
					void initLogicalDevice();
					void initSwapchain();
					void initSemaphores();

					vk::Instance instance;
					std::unique_ptr<DebugReportCallbackEXT> debugEXT;
					vk::SurfaceKHR surface;

					vk::PhysicalDevice gpu;
					vk::Device device;
					vk::Queue presentQueue;
					vk::Queue graphicsQueue;
					
					vk::Semaphore imageAvailable;
					vk::Semaphore renderFinished;

					std::unique_ptr<Swapchain> swapchain = nullptr;

					uint32_t imgIndex = 0;
				};
			}
		}
	}
}
