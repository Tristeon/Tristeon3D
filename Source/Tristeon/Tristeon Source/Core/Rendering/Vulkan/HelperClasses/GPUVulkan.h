#pragma once
#include <vulkan/vulkan.hpp>

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				class GPUVulkan
				{
				public:
					static int rateDevice(vk::PhysicalDevice device);
					static bool isSuitable(vk::PhysicalDevice device, vk::SurfaceKHR surface);
					static vk::PhysicalDevice pickGPU(vk::Instance instance, vk::SurfaceKHR surface);
				};
			}
		}
	}
}