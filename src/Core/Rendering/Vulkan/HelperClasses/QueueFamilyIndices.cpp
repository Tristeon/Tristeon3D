#include "QueueFamilyIndices.h"
#include "Swapchain.h"
#include "Core/BindingData.h"

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

				QueueFamilyIndices QueueFamilyIndices::get(vk::PhysicalDevice physical)
				{
					QueueFamilyIndices result;

					std::vector<vk::QueueFamilyProperties> families = physical.getQueueFamilyProperties();
					int i = 0;
					for (vk::QueueFamilyProperties f : families) 
					{
						//Check if this family supports a graphics queue
						if (f.queueCount > 0 && f.queueFlags & vk::QueueFlagBits::eGraphics)
							result.graphicsFamily = i;

						//Check if this family supports a 
						const vk::Bool32 presentSupport = physical.getSurfaceSupportKHR(i, binding_data.surface);
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
			}
		}
	}
}
