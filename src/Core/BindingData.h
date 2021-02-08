#pragma once
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

namespace Tristeon::Core
{
	struct BindingData
	{
		GLFWwindow* window = nullptr;
		unsigned int width = 0;
		unsigned int height = 0;

		vk::Instance instance = nullptr;
		vk::PhysicalDevice physical = nullptr;
		vk::Device device = nullptr;

		uint32_t graphicsFamily = 0;
		vk::Queue graphicsQueue = nullptr;
		vk::CommandPool graphicsPool = nullptr;

		uint32_t presentFamily = 0;
		vk::Queue presentQueue = nullptr;

		uint32_t transferFamily = 0;
		vk::Queue transferQueue = nullptr;
		vk::CommandPool transferPool = nullptr;
		
		vk::SurfaceKHR surface = nullptr;

		vk::SurfaceFormatKHR surface_format;
		vk::PresentModeKHR present_mode;
		vk::Extent2D extent;

		vk::SwapchainKHR swapchain = nullptr;
		std::vector<vk::Image> swapchain_images{};
		std::vector<vk::ImageView> swapchain_image_views{};
		std::vector<vk::Framebuffer> swapchain_framebuffers;

		vk::RenderPass output_pass = nullptr;
		std::vector<vk::CommandBuffer> command_buffers{};

		vk::DescriptorPool descriptorPool = nullptr;

		const int frames_in_flight = 2;
		std::vector<vk::Semaphore> image_available;
		std::vector<vk::Semaphore> render_finished;
		std::vector<vk::Fence> command_in_flight;
		std::vector<vk::Fence> images_in_flight;

		bool debug_messenger_enabled = true;
		vk::DebugUtilsMessengerEXT messenger;
	};

	inline BindingData binding_data;
}