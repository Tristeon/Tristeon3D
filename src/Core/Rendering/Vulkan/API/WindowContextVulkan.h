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
					explicit WindowContextVulkan(Vulkan::Window* window);
					~WindowContextVulkan();
					void prepareFrame() override;
					void finishFrame() override;

					ReadOnlyProperty(vk::Instance, rop_instance);
					GetPropertyConst(rop_instance) { return instance; }

					ReadOnlyProperty(vk::SurfaceKHR, rop_surface);
					GetPropertyConst(rop_surface) { return surface; }

					ReadOnlyProperty(vk::PhysicalDevice, rop_gpu);
					GetPropertyConst(rop_gpu) { return gpu; }

					ReadOnlyProperty(vk::Device, rop_device);
					GetPropertyConst(rop_device) { return device; }

					ReadOnlyProperty(vk::Queue, rop_presentQueue);
					GetPropertyConst(rop_presentQueue) { return presentQueue; }

					ReadOnlyProperty(vk::Queue, rop_graphicsQueue);
					GetPropertyConst(rop_graphicsQueue) { return graphicsQueue; }

					ReadOnlyProperty(vk::SwapchainKHR, rop_swapchain_khr);
					GetPropertyConst(rop_swapchain_khr) { return swapchain->getSwapchain(); }

					ReadOnlyProperty(Swapchain*, rop_swapchain);
					GetPropertyConst(rop_swapchain) { return swapchain.get(); }

					ReadOnlyProperty(size_t, frameBufferCount);
					GetPropertyConst(frameBufferCount) { return swapchain->getFramebufferCount(); }

					ReadOnlyProperty(vk::Extent2D, rop_extent);
					GetPropertyConst(rop_extent) { return swapchain->extent2D; }
					
					ReadOnlyProperty(vk::RenderPass, rop_renderpass);
					GetPropertyConst(rop_renderpass) { return swapchain->renderpass; }

					ReadOnlyProperty(vk::Semaphore, rop_imageAvailable);
					GetProperty(rop_imageAvailable) { return imageAvailable; }

					ReadOnlyProperty(vk::Semaphore, rop_renderFinished);
					GetProperty(rop_renderFinished) { return renderFinished; }

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
