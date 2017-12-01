#include "VulkanImage.h"
#include "Misc/Console.h"
#include "CommandBuffer.h"
#include "Core/BindingData.h"
#include "VulkanBuffer.h"
#include "VulkanFormat.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				void VulkanImage::createImage(VulkanBindingData* vk, uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, 
					vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory)
				{
					//Create the image with the given parameters
					vk::ImageCreateInfo imgInfo = vk::ImageCreateInfo({},
						vk::ImageType::e2D, format,
						vk::Extent3D(width, height, 1), 1, 1,
						vk::SampleCountFlagBits::e1,
						tiling, usage,
						vk::SharingMode::eExclusive,
						0, nullptr,
						vk::ImageLayout::eUndefined); //Initial layout

					vk::Result r = vk->device.createImage(&imgInfo, nullptr, &image);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to create vulkan image: " + to_string(r));

					//Get memory requirements and memory type
					vk::MemoryRequirements const memReqs = vk->device.getImageMemoryRequirements(image);
					vk::MemoryAllocateInfo alloc = vk::MemoryAllocateInfo(memReqs.size, VulkanBuffer::findMemoryType(vk, memReqs.memoryTypeBits, properties));

					//Allocate memory
					r = vk->device.allocateMemory(&alloc, nullptr, &imageMemory);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to allocate image memory!");

					//Bind image and memory together
					vk->device.bindImageMemory(image, imageMemory, 0);
				}

				vk::ImageView VulkanImage::createImageView(vk::Device device, vk::Image img, vk::Format format, vk::ImageAspectFlags aspectFlags)
				{
					vk::ComponentMapping const components = vk::ComponentMapping(); //Specifies color component mapping
					vk::ImageSubresourceRange const sub = vk::ImageSubresourceRange(aspectFlags, 0, 1, 0, 1); //Aspectmask, baseMipLevel, levelCount, baseArrayLayer, layerCount
					vk::ImageViewCreateInfo viewInfo = vk::ImageViewCreateInfo({}, img, vk::ImageViewType::e2D, format, components, sub);

					//Create image view
					vk::ImageView view;
					vk::Result const r = device.createImageView(&viewInfo, nullptr, &view);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to create texture image view: " + to_string(r));
					return view;
				}

				void VulkanImage::transitionImageLayout(VulkanBindingData* bind, vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
				{
					vk::ImageAspectFlags aspectMask;
					
					//set aspect mask to depth/stencil if the new layout is depthstencil
					if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
					{
						aspectMask = vk::ImageAspectFlagBits::eDepth;
						if (VulkanFormat::hasStencilComponent(format))
							aspectMask |= vk::ImageAspectFlagBits::eStencil;
					}
					//Else it's used for color
					else
						aspectMask = vk::ImageAspectFlagBits::eColor;

					//Narrow down our operation to the respective pipeline stages
					vk::PipelineStageFlagBits src = {};
					vk::PipelineStageFlagBits dst = {};

					vk::ImageSubresourceRange const sub = vk::ImageSubresourceRange(aspectMask, 0, 1, 0, 1);

					//Barriers can be used to describe image transitions
					vk::ImageMemoryBarrier barrier = vk::ImageMemoryBarrier({}, {}, oldLayout, newLayout, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image, sub);

					//Transfer from undefined to transferdst
					if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
					{
						barrier.srcAccessMask = {};
						barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

						src = vk::PipelineStageFlagBits::eTopOfPipe;
						dst = vk::PipelineStageFlagBits::eTransfer;
					}
					//Transfer from transferdst to shaderreadonly
					else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
					{
						barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
						barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

						src = vk::PipelineStageFlagBits::eTransfer;
						dst = vk::PipelineStageFlagBits::eFragmentShader;
					}
					//Transfer from undefined to depthstencilattachment
					else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
					{
						barrier.srcAccessMask = {};
						barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

						src = vk::PipelineStageFlagBits::eTopOfPipe;
						dst = vk::PipelineStageFlagBits::eEarlyFragmentTests;
					}
					else if (oldLayout == vk::ImageLayout::eShaderReadOnlyOptimal && newLayout == vk::ImageLayout::eTransferDstOptimal)
					{
						barrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
						barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

						src = vk::PipelineStageFlagBits::eFragmentShader;
						dst = vk::PipelineStageFlagBits::eTransfer;
					}
					else
					{
						//Terminate
						Misc::Console::error("Transition { from: " + to_string(oldLayout) + " to: " + to_string(newLayout) + " } is not supported!");
					}

					//Transfer
					vk::CommandBuffer const cmd = CommandBuffer::begin(bind->commandPool, bind->device);
					cmd.pipelineBarrier(src, dst, {}, 0, nullptr, 0, nullptr, 1, &barrier);
					CommandBuffer::end(cmd, bind->graphicsQueue, bind->device, bind->commandPool);
				}

				void VulkanImage::copyBufferToImage(VulkanBindingData* vk, vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height)
				{
					//Define what data we need to copy
					vk::ImageSubresourceLayers const layers = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1);

					//Define the image region we're copying to 
					vk::BufferImageCopy region = vk::BufferImageCopy(0, 0, 0, layers, vk::Offset3D(0, 0, 0), vk::Extent3D(width, height, 1));

					//Copy
					vk::CommandBuffer cmd = CommandBuffer::begin(vk->commandPool, vk->device);
					cmd.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);
					CommandBuffer::end(cmd, vk->graphicsQueue, vk->device, vk->commandPool);
				}
			}
		}
	}
}