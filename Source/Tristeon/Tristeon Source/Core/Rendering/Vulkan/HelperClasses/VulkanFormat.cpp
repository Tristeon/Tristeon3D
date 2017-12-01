#include "VulkanFormat.h"
#include "Misc/Console.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				vk::Format VulkanFormat::findDepthFormat(vk::PhysicalDevice physicalDevice)
				{
					//Find a supported format from the given depth formats
					const std::vector<vk::Format> candidates = { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint };
					return findSupportedFormat(physicalDevice, candidates, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
				}

				vk::Format VulkanFormat::findSupportedFormat(vk::PhysicalDevice physicalDevice, const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
				{
					//Go over all candidates
					for (vk::Format f : candidates)
					{
						vk::FormatProperties const props = physicalDevice.getFormatProperties(f);
						//Linear tiling features
						if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
							return f;
						//Optimal tiling features
						if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
							return f;
					}

					Misc::Console::error("Failed to find supported format!");
					return vk::Format::eUndefined;
				}

				bool VulkanFormat::hasStencilComponent(vk::Format format)
				{
					return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
				}
			}
		}
	}
}
