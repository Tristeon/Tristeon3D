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
				/**
				 * \brief Utility class that selects the most optimal graphics and present queue family
				 */
				struct QueueFamilyIndices
				{
					/**
					 * \brief The graphics family
					 */
					int graphicsFamily = -1;
					/**
					 * \brief The present family
					 */
					int presentFamily = -1;

					/**
					 * \return Returns true if both the graphics and the present family have been found 
					 */
					bool isComplete() const;

					/**
					 * \brief Tries to find a suitable graphics and present queue
					 * \param device The physicaldevice, used to get the available queue families
					 * \param surface The surface, used to get the available queue families
					 * \return 
					 */
					static QueueFamilyIndices get(vk::PhysicalDevice device, vk::SurfaceKHR surface);
				};
			}
		}
	}
}