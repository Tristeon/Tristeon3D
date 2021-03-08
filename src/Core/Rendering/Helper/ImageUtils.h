#pragma once
#include <vulkan/vulkan.hpp>

namespace Tristeon::Core::Rendering
{
	class ImageUtils
	{
	public:
		/**
		 * \brief Creates a vulkan image and its memory, and binds them together
		 * \param width The width of the image
		 * \param height The height of the image
		 * \param format The format of the image data
		 * \param tiling Specifies the tiling arrangement of data elements in an image
		 * \param usage Specifies the intended usage of the image
		 * \param properties Indicates the properties for image memory allocation
		 * \param image The vulkan image
		 * \param imageMemory The image's memory
		 */
		static void createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory, int arrayLayers = 1, vk::ImageCreateFlags flags = {});

		/**
		 * \brief Transitions the layout of the given image from one layout to another
		 */
		static void transitionImageLayout(vk::Image image, vk::ImageSubresourceRange subresource,
		                                  vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage,
		                                  vk::ImageLayout srcLayout, vk::ImageLayout dstLayout,
										  vk::AccessFlags srcAccess, vk::AccessFlags dstAccess,
		                                  uint32_t srcFamily = VK_QUEUE_FAMILY_IGNORED, uint32_t dstFamily = VK_QUEUE_FAMILY_IGNORED
		);

		/**
		 * \brief Copies buffer data to an image
		 * \param buffer The buffer containing the to-be-copied data
		 * \param image The image receiving the data
		 * \param width The width of the image
		 * \param height The height of the image
		 */
		static void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);
	};
}