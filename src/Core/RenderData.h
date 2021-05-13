#pragma once
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

namespace Tristeon::Core
{
	struct FrameBufferAttachment
	{
		vk::Image image = nullptr;
		vk::DeviceMemory memory = nullptr;
		vk::ImageView view = nullptr;
		vk::Sampler sampler = nullptr;

		void destroy(vk::Device d)
		{
			d.destroyImage(image);
			d.freeMemory(memory);
			d.destroyImageView(view);
			d.destroySampler(sampler);
		}
	};
	
	struct RenderData
	{
		constexpr static uint8_t FRAMES_IN_FLIGHT = 2;
		
		GLFWwindow* window = nullptr;

		//Core
		vk::Instance instance = nullptr;
		vk::PhysicalDevice physical = nullptr;
		vk::Device device = nullptr;

		bool debugMessengerEnabled = true;
		vk::DebugUtilsMessengerEXT debugMessenger;

		struct Queue
		{
			uint32_t family = 0;
			vk::Queue queue = nullptr;
			vk::CommandPool pool = nullptr;
		};
		Queue graphics, present, transfer, compute;
		
		vk::SurfaceKHR surface = nullptr;

		vk::SurfaceFormatKHR format{ vk::Format::eR8G8B8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear };
		vk::PresentModeKHR presentMode = vk::PresentModeKHR::eMailbox;
		vk::Extent2D extent{ 800, 800 };

		vk::SwapchainKHR swapchain = nullptr;
		std::vector<vk::Image> swapchainImages;
		std::vector<vk::ImageView> swapchainImageViews;
		std::vector<vk::Framebuffer> swapchainFramebuffers;
		std::vector<vk::CommandBuffer> onscreenCmds;
		
		struct Frame
		{
			vk::CommandBuffer offscreen = nullptr;
			
			//Sync
			vk::Semaphore imageAvailable = nullptr; //Swapchain ready for next frame
			vk::Semaphore offscreenFinished = nullptr; //Offscreen done
			vk::Semaphore onscreenFinished = nullptr; //Onscreen done

			vk::Fence fence = nullptr;
			
			//Offscreen
			vk::Framebuffer offscreenFramebuffer = nullptr;
			FrameBufferAttachment offscreenColor, offscreenNormal, offscreenDepth;
		};
		std::array<Frame, FRAMES_IN_FLIGHT> frame;

		vk::DescriptorPool descriptorPool = nullptr;

		//Offscreen pass
		vk::RenderPass offscreenPass = nullptr;
		vk::DescriptorSetLayout transformLayout = nullptr;
		
		//Onscreen pass
		vk::RenderPass outputPass = nullptr;
	};

	inline RenderData renderData;
}