#include "VulkanCore.h"

#include "Misc/Console.h"

#include <vector>

#include "VulkanExtensions.h"
#include "ValidationLayers.h"
#include <vulkan/vulkan.hpp>
#include "Device.h"
#include "Core/BindingData.h"
#include <glfw/glfw3.h>

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				VulkanCore::VulkanCore(VulkanBindingData* binding) : binding(binding), window(binding->window)
				{
					createInstance();
					setupDebugCallback();
					createWindowSurface();
					dev = new Device(instance, surface);

					storeBindingData();
				}

				VulkanCore::~VulkanCore()
				{
					//Cleanup
					instance.destroySurfaceKHR(surface);
					delete dev;
					delete debug;
					instance.destroy(nullptr);
				}

				VulkanCore::property__tmp_type_device VulkanCore::property__get_device() const
				{
					return dev->getLogical();
				}

				VulkanCore::property__tmp_type_physicalDevice VulkanCore::property__get_physicalDevice() const
				{
					return dev->getPhysical();
				}

				VulkanCore::property__tmp_type_graphicsQueue VulkanCore::property__get_graphicsQueue() const
				{
					return dev->getGraphicsQueue();
				}

				VulkanCore::property__tmp_type_presentQueue VulkanCore::property__get_presentQueue() const
				{
					return dev->getPresentQueue();
				}

				void VulkanCore::createInstance()
				{
					//Layers
					if (ValidationLayers::enabled() && !ValidationLayers::supported())
						Misc::Console::error("Validation layers requested but not available!");

					//Enabled extensions
					std::vector<const char*> extensions = VulkanExtensions::getRequiredExtensions();

					//AppInfo
					vk::ApplicationInfo appInfo = vk::ApplicationInfo("Tristeon", VK_MAKE_VERSION(2, 0, 0), "Tristeon", VK_MAKE_VERSION(2, 0, 0), VK_API_VERSION_1_0);

					//Instance Create Info
					const int layerCount = ValidationLayers::enabled() ? ValidationLayers::validationLayers.size() : 0;
					const char* const* layerNames = ValidationLayers::enabled() ? ValidationLayers::validationLayers.data() : nullptr;
					vk::InstanceCreateInfo instanceCI = vk::InstanceCreateInfo({}, &appInfo, layerCount, layerNames, extensions.size(), extensions.data());

					//Create Instance
					const vk::Result r = vk::createInstance(&instanceCI, nullptr, &instance);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to create Vulkan Instance. Error: " + to_string(r));
				}

				void VulkanCore::setupDebugCallback()
				{
					//Only if we are actually using validation layers
					if (!ValidationLayers::enabled())
						return;

					//Setup debug
					debug = new DebugReportCallbackEXT(instance);
					Misc::Console::t_assert(debug->getResult() == vk::Result::eSuccess, "Failed to setup debug callback. Error: " + to_string(debug->getResult()));
				}

				void VulkanCore::createWindowSurface()
				{
					//Create window khr surface using GLFW's helper function
					VkSurfaceKHR psurf = VK_NULL_HANDLE;
					const VkResult r = glfwCreateWindowSurface(VkInstance(instance), window, nullptr, &psurf);
					surface = vk::SurfaceKHR(psurf);
					Misc::Console::t_assert(r == VK_SUCCESS, "Failed to create window surface!");
				}

				void VulkanCore::storeBindingData() const
				{
					binding->physicalDevice = dev->getPhysical();
					binding->device = dev->getLogical();
					binding->graphicsQueue = dev->getGraphicsQueue();
					binding->presentQueue = dev->getPresentQueue();
				}
			}
		}
	}
}
