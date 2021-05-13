#include "DeviceSelector.h"

#include <set>

#include "Core/RenderData.h"
#include "Misc/Console.h"
#include <Core/Rendering/RenderManager.h>

namespace Tristeon::Core::Rendering
{
	vk::PhysicalDeviceMemoryProperties DeviceSelector::memoryProperties;
	
	vk::SwapchainKHR DeviceSelector::swapchain()
	{
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating swapchain");

		const auto formats = renderData.physical.getSurfaceFormatsKHR(renderData.surface);
		renderData.format = formats[0];
		for (const auto& format : formats)
		{
			if (format.format == vk::Format::eB8G8R8A8Srgb && //SRGB results in more accurate colors
				format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
				renderData.format = format;
		}
		Misc::Console::write("\tImage format: " + vk::to_string(renderData.format.format) + " with color space: " + vk::to_string(renderData.format.colorSpace));

		const auto modes = renderData.physical.getSurfacePresentModesKHR(renderData.surface);
		renderData.presentMode = vk::PresentModeKHR::eFifo;
		for (const auto mode : modes)
		{
			if (mode == vk::PresentModeKHR::eMailbox)
				renderData.presentMode = mode;
		}
		Misc::Console::write("\tPresent mode: " + vk::to_string(renderData.presentMode));

		const auto capabilities = renderData.physical.getSurfaceCapabilitiesKHR(renderData.surface);
		if (capabilities.currentExtent.width != UINT32_MAX)
			renderData.extent = capabilities.currentExtent;
		else
		{
			int w = 0;
			int h = 0;
			glfwGetFramebufferSize(renderData.window, &w, &h);

			renderData.extent = vk::Extent2D{
				std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, (unsigned int)w)),
				std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, (unsigned int)h))
			};
		}
		Misc::Console::write("\tSwap extent: " + std::to_string(renderData.extent.width) + "x" + std::to_string(renderData.extent.height));

		uint32_t image_count = capabilities.minImageCount + 1;
		if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount)
			image_count = capabilities.maxImageCount;
		Misc::Console::write("\tImage count: " + std::to_string(image_count));

		std::array<uint32_t, 2> families{ renderData.graphics.family, renderData.present.family };
		Misc::Console::write("\tSwapchain operating in " + (std::string)((renderData.graphics.family == renderData.present.family) ? "exclusive" : "concurrent") + " mode");

		const vk::SwapchainCreateInfoKHR swapchain_ci(
			{},
			renderData.surface,
			image_count,
			renderData.format.format,
			renderData.format.colorSpace,
			renderData.extent,
			1, //Image array layers, always 1 unless for stereoscopic 3D apps 
			vk::ImageUsageFlagBits::eColorAttachment,
			//Exclusive is more performant but handling multiple queues takes extra steps
			renderData.graphics.family != renderData.present.family ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive,
			renderData.graphics.family != renderData.present.family ? 0 : 2,
			renderData.graphics.family != renderData.present.family ? families.data() : nullptr,
			capabilities.currentTransform, //Should images be transformed? (e.g. flipped or rotated) - current does nothing
			vk::CompositeAlphaFlagBitsKHR::eOpaque,
			renderData.presentMode,
			VK_TRUE, nullptr);

		return renderData.device.createSwapchainKHR(swapchain_ci);
	}

	vk::PhysicalDevice DeviceSelector::select()
	{
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Selecting physical device from the following:");

		uint32_t count = 0;
		VULKAN_DEBUG(renderData.instance.enumeratePhysicalDevices(&count, nullptr));

		std::vector<vk::PhysicalDevice> devices(count);
		VULKAN_DEBUG(renderData.instance.enumeratePhysicalDevices(&count, devices.data()));

		if (count == 0) {
			Misc::Console::error("[RENDERER] [ERROR] [VULKAN] No GPU with Vulkan support found!");
			throw std::runtime_error("No GPU with Vulkan support");
		}

		//Select the highest scoring gpu
		int lowest = -1;
		vk::PhysicalDevice selected;
		for (auto physical : devices)
		{
			auto properties = physical.getProperties();

			Misc::Console::write("\t" + (std::string)properties.deviceName.data());
			if (suitable(physical))
			{
				const int rating = rate(physical);
				Misc::Console::write("\t\tDevice is suitable. Rating: " + std::to_string(rating));

				if (rating > 0 && rating > lowest)
				{
					lowest = rating;
					selected = physical;
				}
			}
			else
			{
				Misc::Console::write("\t\tDevice is not suitable.");
			}
		}

		if (lowest == -1)
		{
			Misc::Console::error("[RENDERER] [ERROR] [VULKAN] No viable GPU found!");
			throw std::runtime_error("No viable GPU found");
		}

		const auto properties = selected.getProperties();
		memoryProperties = selected.getMemoryProperties();
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Selected GPU: " + (std::string)properties.deviceName.data());
		return selected;
	}

	uint32_t DeviceSelector::findGraphicsFamily()
	{
		auto families = renderData.physical.getQueueFamilyProperties();

		for (int i = 0; i < families.size(); ++i)
		{
			if (families[i].queueFlags & vk::QueueFlagBits::eGraphics)
				return i;
		}

		return -1;
	}

	uint32_t DeviceSelector::findPresentFamily(uint32_t graphicsFamily)
	{
		//Ideally graphics & present family are the same
		if (renderData.physical.getSurfaceSupportKHR(graphicsFamily, renderData.surface))
			return graphicsFamily;

		//If not, just find any that works
		const auto families = renderData.physical.getQueueFamilyProperties();
		for (int i = 0; i < families.size(); ++i)
		{
			if (renderData.physical.getSurfaceSupportKHR(i, renderData.surface))
				return i;
		}
		
		Misc::Console::error("[RENDERER] [INIT] [VULKAN] No queue with surface support found!");
		throw std::runtime_error("No queue with surface support found!");
	}

	uint32_t DeviceSelector::findTransferFamily()
	{
		auto families = renderData.physical.getQueueFamilyProperties();

		//Ideally we find an exclusive transfer queue
		for (int i = 0; i < families.size(); ++i)
		{
			if ((families[i].queueFlags & vk::QueueFlagBits::eTransfer) == vk::QueueFlagBits::eTransfer &&
				(families[i].queueFlags & vk::QueueFlagBits::eGraphics) == vk::QueueFlags{} &&
				(families[i].queueFlags & vk::QueueFlagBits::eCompute) == vk::QueueFlags{})
				return i;
		}

		//Otherwise any transfer compatible queue will do
		for (int i = 0; i < families.size(); ++i)
		{
			if (families[i].queueFlags & vk::QueueFlagBits::eTransfer)
				return i;
		}

		return -1;
	}

	uint32_t DeviceSelector::findComputeFamily()
	{
		auto families = renderData.physical.getQueueFamilyProperties();

		//Ideally we find an exclusive transfer queue
		for (int i = 0; i < families.size(); ++i)
		{
			if ((families[i].queueFlags & vk::QueueFlagBits::eCompute) == vk::QueueFlagBits::eCompute &&
				(families[i].queueFlags & vk::QueueFlagBits::eGraphics) == vk::QueueFlags{} &&
				(families[i].queueFlags & vk::QueueFlagBits::eTransfer) == vk::QueueFlags{})
				return i;
		}

		//Otherwise any transfer compatible queue will do
		for (int i = 0; i < families.size(); ++i)
		{
			if (families[i].queueFlags & vk::QueueFlagBits::eCompute)
				return i;
		}

		return -1;
	}

	uint32_t DeviceSelector::getMemoryType(uint32_t typeBits, vk::MemoryPropertyFlags properties)
	{
		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
		{
			if ((typeBits & 1) == 1)
			{
				if ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
				{
					return i;
				}
			}
			typeBits >>= 1;
		}

		throw std::runtime_error("Could not find a matching memory type");
	}

	bool DeviceSelector::hasStencilComponent(vk::Format format)
	{
		return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
	}

	int DeviceSelector::rate(vk::PhysicalDevice physical)
	{
		const vk::PhysicalDeviceFeatures features = physical.getFeatures();
		if (!features.geometryShader)
			return -1;

		int score = 1;

		const vk::PhysicalDeviceProperties properties = physical.getProperties();
		if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			score += 1000;

		return score;
	}

	bool DeviceSelector::suitable(vk::PhysicalDevice physical)
	{
		const bool extensionsSupported = supports_extensions(physical);

		if (extensionsSupported)
		{
			const auto formats = physical.getSurfaceFormatsKHR(renderData.surface);
			const auto modes = physical.getSurfacePresentModesKHR(renderData.surface);
			return !formats.empty() && !modes.empty();
		}

		return extensionsSupported;
	}

	bool DeviceSelector::supports_extensions(vk::PhysicalDevice physical)
	{
		auto available = physical.enumerateDeviceExtensionProperties();

		//Iterate over the available set of extensions and see if every required extension is in there
		Misc::Console::write("\t\tAvailable extensions:");
		std::set<std::string> requiredExtensions(gpuExtensions.begin(), gpuExtensions.end());
		for (const auto& extension : available) {
			Misc::Console::write("\t\t\t" + (std::string)extension.extensionName.data());
			requiredExtensions.erase(extension.extensionName.data());
			
			if (requiredExtensions.empty())
				break;
		}
		return requiredExtensions.empty();
	}
}