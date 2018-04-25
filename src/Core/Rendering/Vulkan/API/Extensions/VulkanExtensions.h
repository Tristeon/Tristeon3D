#pragma once
#include <vector>

//Forward decl
namespace vk { class PhysicalDevice; }
namespace Tristeon 
{
	namespace Core 
	{
		namespace Rendering 
		{
			namespace Vulkan 
			{
				/**
				 * \brief Helper class for device extension checking
				 */
				class VulkanExtensions
				{
				public:
					/**
					 * \return Returns the extensions that we required physical devices to support
					 */
					static std::vector<const char*> getRequiredExtensions();
					/**
					 * \brief Checks if the given physical device supports all the required extensions
					 * \param device The physical device to be checked
					 * \return True if the extensions are supported
					 */
					static bool checkDeviceExtensionSupport(vk::PhysicalDevice device);
				
					/**
					 * \brief All the required device extension
					 */
					const static std::vector<const char*> deviceExtensions;
				};
			}
		}
	}
}
