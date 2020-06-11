#include "VulkanExtensions.h"
#include "ValidationLayers.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <set>

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				const std::vector<const char*> VulkanExtensions::deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

				std::vector<const char*> VulkanExtensions::getRequiredExtensions()
				{
					std::vector<const char*> extensions;

					//Get required GLFW extensions
					unsigned int glfwExtensionCount = 0;
					const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
					for (unsigned int i = 0; i < glfwExtensionCount; i++)
						extensions.push_back(glfwExtensions[i]);

					//If validation layers are enabled we should also be supporting DebugReportEXT
					if (ValidationLayers::enabled() && ValidationLayers::supported())
						extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

					return extensions;
				}

				bool VulkanExtensions::checkDeviceExtensionSupport(vk::PhysicalDevice device)
				{
					//Available extensions
					std::vector<vk::ExtensionProperties> available = device.enumerateDeviceExtensionProperties();

					//List all required extensions
					std::set<std::string> required(deviceExtensions.begin(), deviceExtensions.end());

					//Remove every extension that we've met from the required list
					for (const auto& extension : available)
						required.erase(extension.extensionName);

					//Return true if there's no required extensions left over
					return required.empty();
				}
			}
		}
	}
}