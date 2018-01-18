#pragma once
#include <vulkan/vulkan.hpp>

//Forward decl
namespace vk {
	class DeviceMemory;
	enum class Format;
	enum class ImageLayout;
	class Buffer;
	class Image;
}

namespace Tristeon
{
	namespace Core
	{
		//Forward decl
		class VulkanBindingData;

		namespace Rendering
		{
			namespace Vulkan
			{
				/**
				 * \brief VulkanImage is a helper class for creating/modifying vulkan images and their respective components
				 */
				class VulkanImage
				{
				public:
					/**
					 * \brief Creates a vulkan image and its memory, and binds them together
					 * \param bind Rendering data
					 * \param width The width of the image
					 * \param height The height of the image
					 * \param format The format of the image data
					 * \param tiling Specifies the tiling arrangement of data elements in an image
					 * \param usage Specifies the intended usage of the image
					 * \param properties Indicates the properties for image memory allocation
					 * \param image The vulkan image
					 * \param imageMemory The image's memory
					 */
					static void createImage(VulkanBindingData* bind, uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory, int arrayLayers = 1, vk::ImageCreateFlags flags = {});
					/**
					 * \brief Creates a vulkan image view for the given vulkan image
					 * \param device The vulkan logical device, for creation of the image view
					 * \param img The respective image
					 * \param format The format of the image (and its view)
					 * \param aspectFlags Specifies which aspects of the image are included in the view
					 * \return Returns the resulting image view
					 */
					static vk::ImageView createImageView(vk::Device device, vk::Image img, vk::Format format, vk::ImageAspectFlags aspectFlags, vk::ImageViewType viewType = vk::ImageViewType::e2D);
					/**
					 * \brief Transitions the layout of the given image from one layout to another
					 * \param bind Rendering data
					 * \param oldLayout The current image layout
					 * \param newLayout The image layout we wish to get
					 */
					static void transitionImageLayout(VulkanBindingData* bind, vk::Image, vk::Format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageSubresourceRange subresource_range = vk::ImageSubresourceRange({}, 0, 1, 0, 1));
					/**
					 * \brief Copies buffer data to an image
					 * \param bind Rendering data
					 * \param buffer The buffer containing the to-be-copied data
					 * \param image The image receiving the data
					 * \param width The width of the image
					 * \param height The height of the image
					 */
					static void copyBufferToImage(VulkanBindingData* bind, vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);
				};
			}
		}
	}
}
