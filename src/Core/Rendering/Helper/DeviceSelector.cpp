#include "DeviceSelector.h"

#include <set>

#include "Core/BindingData.h"
#include "Misc/Console.h"
#include <Core/Rendering/RenderManager.h>

namespace Tristeon::Core::Rendering
{
	vk::SwapchainKHR DeviceSelector::swapchain()
	{
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating swapchain");

		const auto formats = binding_data.physical.getSurfaceFormatsKHR(binding_data.surface);
		binding_data.format = formats[0];
		for (const auto& format : formats)
		{
			if (format.format == vk::Format::eB8G8R8A8Srgb && //SRGB results in more accurate colors
				format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
				binding_data.format = format;
		}
		Misc::Console::write("\tImage format: " + vk::to_string(binding_data.format.format) + " with color space: " + vk::to_string(binding_data.format.colorSpace));

		const auto modes = binding_data.physical.getSurfacePresentModesKHR(binding_data.surface);
		binding_data.presentMode = vk::PresentModeKHR::eFifo;
		for (const auto mode : modes)
		{
			if (mode == vk::PresentModeKHR::eMailbox)
				binding_data.presentMode = mode;
		}
		Misc::Console::write("\tPresent mode: " + vk::to_string(binding_data.presentMode));

		const auto capabilities = binding_data.physical.getSurfaceCapabilitiesKHR(binding_data.surface);
		if (capabilities.currentExtent.width != UINT32_MAX)
			binding_data.extent = capabilities.currentExtent;
		else
		{
			int w = 0;
			int h = 0;
			glfwGetFramebufferSize(binding_data.window, &w, &h);

			binding_data.extent = vk::Extent2D{
				std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, (unsigned int)w)),
				std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, (unsigned int)h))
			};
		}
		Misc::Console::write("\tSwap extent: " + std::to_string(binding_data.extent.width) + "x" + std::to_string(binding_data.extent.height));

		uint32_t image_count = capabilities.minImageCount + 1;
		if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount)
			image_count = capabilities.maxImageCount;
		Misc::Console::write("\tImage count: " + std::to_string(image_count));

		std::array<uint32_t, 2> families{ binding_data.graphicsFamily, binding_data.presentFamily };
		Misc::Console::write("\tSwapchain operating in " + (std::string)((binding_data.graphicsFamily == binding_data.presentFamily) ? "exclusive" : "concurrent") + " mode");

		const vk::SwapchainCreateInfoKHR swapchain_ci(
			{},
			binding_data.surface,
			image_count,
			binding_data.format.format,
			binding_data.format.colorSpace,
			binding_data.extent,
			1, //Image array layers, always 1 unless for stereoscopic 3D apps 
			vk::ImageUsageFlagBits::eColorAttachment,
			//Exclusive is more performant but handling multiple queues takes extra steps
			binding_data.graphicsFamily != binding_data.presentFamily ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive,
			binding_data.graphicsFamily != binding_data.presentFamily ? 0 : 2,
			binding_data.graphicsFamily != binding_data.presentFamily ? families.data() : nullptr,
			capabilities.currentTransform, //Should images be transformed? (e.g. flipped or rotated) - current does nothing
			vk::CompositeAlphaFlagBitsKHR::eOpaque,
			binding_data.presentMode,
			VK_TRUE, nullptr);

		return binding_data.device.createSwapchainKHR(swapchain_ci);
	}

	vk::PhysicalDevice DeviceSelector::select()
	{
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Selecting physical device from the following:");

		uint32_t count = 0;
		VULKAN_DEBUG(binding_data.instance.enumeratePhysicalDevices(&count, nullptr));

		std::vector<vk::PhysicalDevice> devices(count);
		VULKAN_DEBUG(binding_data.instance.enumeratePhysicalDevices(&count, devices.data()));

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
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Selected GPU: " + (std::string)properties.deviceName.data());
		return selected;
	}

	uint32_t DeviceSelector::findGraphicsFamily()
	{
		auto families = binding_data.physical.getQueueFamilyProperties();

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
		if (binding_data.physical.getSurfaceSupportKHR(graphicsFamily, binding_data.surface))
			return graphicsFamily;

		//If not, just find any that works
		const auto families = binding_data.physical.getQueueFamilyProperties();
		for (int i = 0; i < families.size(); ++i)
		{
			if (binding_data.physical.getSurfaceSupportKHR(i, binding_data.surface))
				return i;
		}
		
		Misc::Console::error("[RENDERER] [INIT] [VULKAN] No queue with surface support found!");
		throw std::runtime_error("No queue with surface support found!");
	}

	uint32_t DeviceSelector::findTransferFamily()
	{
		auto families = binding_data.physical.getQueueFamilyProperties();

		//Ideally we find an exclusive transfer queue
		for (int i = 0; i < families.size(); ++i)
		{
			if (families[i].queueFlags == vk::QueueFlagBits::eTransfer)
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
			const auto formats = physical.getSurfaceFormatsKHR(binding_data.surface);
			const auto modes = physical.getSurfacePresentModesKHR(binding_data.surface);
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