#pragma once
#include <vulkan/vulkan.hpp>

namespace Tristeon::Core::Rendering
{
	class DeviceSelector
	{
	public:
		static vk::SwapchainKHR swapchain();
		
		static vk::PhysicalDevice select();
		static uint32_t findGraphicsFamily();
		static uint32_t findPresentFamily(uint32_t graphicsFamily);
		static uint32_t findTransferFamily();

		constexpr static std::array<char*, 1> gpuExtensions{ "VK_KHR_swapchain" };

		static uint32_t getMemoryType(uint32_t typeBits, vk::MemoryPropertyFlags properties);
		static bool hasStencilComponent(vk::Format format);
	private:
		static int rate(vk::PhysicalDevice physical);
		static bool suitable(vk::PhysicalDevice physical);
		static bool supports_extensions(vk::PhysicalDevice physical);

		static vk::PhysicalDeviceMemoryProperties memoryProperties;
	};
}