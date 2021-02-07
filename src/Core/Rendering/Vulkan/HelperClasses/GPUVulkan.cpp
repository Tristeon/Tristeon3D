#include "GPUVulkan.h"
#include "QueueFamilyIndices.h"
#include "../API/Extensions/VulkanExtensions.h"
#include "Swapchain.h"
#include <map>
#include "Misc/Console.h"

namespace Tristeon
{
	namespace Core
	{
		class VulkanBindingData;

		namespace Rendering
		{
			namespace Vulkan
			{
				int GPUVulkan::rateDevice(vk::PhysicalDevice device)
				{
					//Get data
					const vk::PhysicalDeviceProperties properties = device.getProperties();
					const vk::PhysicalDeviceFeatures features = device.getFeatures();

					int score = 0;

					//Discrete gpus have a huge performance increase over integrated gpus
					if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
						score += 1000;

					//Increases texture quality
					score += properties.limits.maxImageDimension2D;

					//We NEED a geometryshader
					if (!features.geometryShader)
						return 0;

					return score;
				}

				bool GPUVulkan::isSuitable(vk::PhysicalDevice device, vk::SurfaceKHR surface)
				{
					//We need to have both queue families available
					if (!QueueFamilyIndices::get().isComplete())
						return false;

					//Check if the swapchain supports all the extensions we need (SurfaceKHR in this case)
					if (!VulkanExtensions::checkDeviceExtensionSupport(device))
						return false;

					//Make sure that the swapchain supports at least one format and one presentmode
					SwapChainSupportDetails swapChain = SwapChainSupportDetails(device, surface);
					if (swapChain.formats.empty() || swapChain.presentModes.empty())
						return false;

					//Make sure we support anisotropy filtering and widelines
					vk::PhysicalDeviceFeatures const features = device.getFeatures();
					if (!features.samplerAnisotropy || !features.wideLines)
						return false;

					//Everything works as intended
					return true;
				}

				vk::PhysicalDevice GPUVulkan::pickGPU(vk::Instance instance, vk::SurfaceKHR surface)
				{
					//Get candidates
					std::vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();
					std::multimap<int, vk::PhysicalDevice> candidates;

					//Get all suitable devices and rate them
					for (vk::PhysicalDevice pd : devices)
						if (isSuitable(pd, surface))
							candidates.insert(std::make_pair(rateDevice(pd), pd));

					//Get the GPU with the highest score
					if (candidates.rbegin()->first > 0)
						return candidates.rbegin()->second;
					
					Misc::Console::error("Failed to find suitable GPU!");
					return nullptr;
				}
			}
		}
	}
}
