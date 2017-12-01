#include "Device.h"
#include "Misc/Console.h"
#include <map>
#include "ValidationLayers.h"
#include <set>
#include "VulkanExtensions.h"
#include "Swapchain.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				bool QueueFamilyIndices::isComplete() const
				{
					return graphicsFamily >= 0 && presentFamily >= 0;
				}

				QueueFamilyIndices QueueFamilyIndices::get(vk::PhysicalDevice device, vk::SurfaceKHR surface)
				{
					QueueFamilyIndices result;

					std::vector<vk::QueueFamilyProperties> families = device.getQueueFamilyProperties();
					int i = 0;
					for (vk::QueueFamilyProperties f : families) 
					{
						//Check if this family supports a graphics queue
						if (f.queueCount > 0 && f.queueFlags & vk::QueueFlagBits::eGraphics)
							result.graphicsFamily = i;

						//Check if this family supports a 
						const vk::Bool32 presentSupport = device.getSurfaceSupportKHR(i, surface);
						if (f.queueCount > 0 && presentSupport)
							result.presentFamily = i;

						//Got a working result?
						//TODO: Selecting a queue that has both graphics and present on i is faster in performance.
						if (result.isComplete())
							break;

						i++;
					}

					return result;
				}

				Device::Device(vk::Instance instance, vk::SurfaceKHR surface)
				{
					getPhysical(instance, surface);
					createLogical(surface);
				}

				Device::~Device()
				{
					device.destroy();
				}

				int Device::rateDevice(vk::PhysicalDevice device)
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

				bool Device::isSuitable(vk::PhysicalDevice device, vk::SurfaceKHR surface)
				{
					//We need to have both queue families available
					if (!QueueFamilyIndices::get(device, surface).isComplete())
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

				void Device::getPhysical(vk::Instance instance, vk::SurfaceKHR surface)
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
						physicalDevice = candidates.rbegin()->second;
					else
						Misc::Console::error("Failed to find suitable GPU!");
				}

				void Device::createLogical(vk::SurfaceKHR surface)
				{
					//Queue create info
					const QueueFamilyIndices indices = QueueFamilyIndices::get(physicalDevice, surface);

					std::vector<vk::DeviceQueueCreateInfo> qcis;
					std::set<int> uniqueFamilies = { indices.graphicsFamily, indices.presentFamily };

					//Queues
					const float queuePriority = 1.0f;
					for (int queueFamily : uniqueFamilies)
					{
						const vk::DeviceQueueCreateInfo qci = vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), queueFamily, 1, &queuePriority);
						qcis.push_back(qci);
					}

					//Set device info
					vk::PhysicalDeviceFeatures features = {};
					features.samplerAnisotropy = VK_TRUE;
					features.wideLines = VK_TRUE;

					//Layers
					const uint32_t enabledLayerCount = ValidationLayers::validationLayers.size();
					const char * const * enabledLayers = ValidationLayers::validationLayers.data();

					//Extensions
					const int extensionCount = VulkanExtensions::deviceExtensions.size();
					const char* const * enabledExtensions = VulkanExtensions::deviceExtensions.data();

					//Create
					vk::DeviceCreateInfo dci = vk::DeviceCreateInfo(vk::DeviceCreateFlags(), qcis.size(), qcis.data(), enabledLayerCount, enabledLayers, extensionCount, enabledExtensions, &features);
					const vk::Result r = physicalDevice.createDevice(&dci, nullptr, &device);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to create logical device!");

					//Receive queues
					graphicsQueue = device.getQueue(indices.graphicsFamily, 0);
					presentQueue = device.getQueue(indices.presentFamily, 0);
				}
			}
		}
	}
}
