#include "ImageUtils.h"
#include <Core/BindingData.h>
#include <Misc/Console.h>

#include "DeviceSelector.h"
#include "OneTimeCmd.h"

namespace Tristeon::Core::Rendering
{
	void ImageUtils::createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling,
		vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory,
		int arrayLayers, vk::ImageCreateFlags flags)
	{
		//Create the image with the given parameters
		vk::ImageCreateInfo imgInfo = vk::ImageCreateInfo(flags,
			vk::ImageType::e2D, format,
			vk::Extent3D(width, height, 1), 1, arrayLayers,
			vk::SampleCountFlagBits::e1,
			tiling, usage,
			vk::SharingMode::eExclusive,
			0, nullptr,
			vk::ImageLayout::eUndefined); //Initial layout

		vk::Result r = binding_data.device.createImage(&imgInfo, nullptr, &image);
		Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to create vulkan image: " + to_string(r));

		//Get memory requirements and memory type
		vk::MemoryRequirements const memReqs = binding_data.device.getImageMemoryRequirements(image);
		vk::MemoryAllocateInfo alloc = vk::MemoryAllocateInfo(memReqs.size, DeviceSelector::getMemoryType(memReqs.memoryTypeBits, properties));

		//Allocate memory
		r = binding_data.device.allocateMemory(&alloc, nullptr, &imageMemory);
		Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to allocate image memory!");

		//Bind image and memory together
		binding_data.device.bindImageMemory(image, imageMemory, 0);
	}
	
	void ImageUtils::transitionImageLayout( vk::Image image, vk::ImageSubresourceRange subresource, 
											vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage,
											vk::ImageLayout srcLayout, vk::ImageLayout dstLayout,
											vk::AccessFlags srcAccess, vk::AccessFlags dstAccess,
											uint32_t srcFamily, uint32_t dstFamily)
	{
		vk::ImageMemoryBarrier barrier = vk::ImageMemoryBarrier(srcAccess, dstAccess, srcLayout, dstLayout, srcFamily, dstFamily, image, subresource);

		//Transfer
		vk::CommandBuffer const cmd = OneTimeCmd::begin(binding_data.graphicsPool);
		cmd.pipelineBarrier(srcStage, dstStage, {}, {}, {}, barrier);
		OneTimeCmd::end(cmd, binding_data.graphicsQueue, binding_data.graphicsPool);
	}

	void ImageUtils::copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height)
	{
		auto const layers = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1);
		auto region = vk::BufferImageCopy(0, 0, 0, layers, vk::Offset3D(0, 0, 0), vk::Extent3D(width, height, 1));

		const auto cmd = OneTimeCmd::begin(binding_data.graphicsPool);
		cmd.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);
		OneTimeCmd::end(cmd, binding_data.graphicsQueue, binding_data.graphicsPool);
	}
}
