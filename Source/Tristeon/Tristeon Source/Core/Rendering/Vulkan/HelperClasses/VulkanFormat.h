#pragma once
#include <vulkan/vulkan.hpp>

namespace vk { enum class Format; }

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				/**
				 * \brief Helper class for Vulkan formats
				 */
				class VulkanFormat
				{
				public:
					/**
					 * \brief Finds the	most suitable depth format that the physical device has got available
					 * \param physicalDevice The physical device to be checked
					 * \return Returns a depth format
					 */
					static vk::Format findDepthFormat(vk::PhysicalDevice physicalDevice);
					/**
					 * \brief Gets the first format that this physical device supports, out of the list of candidates
					 * \param physicalDevice The physical device
					 * \param candidates The list of candidates to be checked
					 * \param tiling Required tiling settings
					 * \param features Required features
					 * \return Returns the first format that supports all the requirements
					 */
					static vk::Format findSupportedFormat(vk::PhysicalDevice physicalDevice, const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
					/**
					 * \brief Returns true if the given format has a stencil component
					 * \param format The format to be checked
					 * \return True if the given format has a stencil component
					 */
					static bool hasStencilComponent(vk::Format format);
				};
			}
		}
	}
}