#include "BufferVulkan.h"
#include <vulkan/vulkan.hpp>

#include "../HelperClasses/CommandBuffer.h"
#include "../HelperClasses/QueueFamilyIndices.h"

#include <Misc/Console.h>

#include "Core/BindingData.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				BufferVulkan::BufferVulkan(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::SharingMode sharingMode) : size(size)
				{
					//Create a vulkan buffer with our given settings
					QueueFamilyIndices const indices = QueueFamilyIndices::get(binding_data.physical);
					uint32_t families[] = { indices.graphicsFamily, indices.presentFamily };

					vk::BufferCreateInfo ci = vk::BufferCreateInfo({}, size, usage, sharingMode, 2, families);
					vk::Result r = binding_data.device.createBuffer(&ci, nullptr, &buffer);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to create buffer: " + to_string(r));

					//Allocate VRAM accordingly
					vk::MemoryRequirements const memReq = binding_data.device.getBufferMemoryRequirements(buffer);
					uint32_t const memType = findMemoryType(memReq.memoryTypeBits, properties);
					vk::MemoryAllocateInfo mallocInfo = vk::MemoryAllocateInfo(memReq.size, memType);
						
					r = binding_data.device.allocateMemory(&mallocInfo, nullptr, &memory);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to allocate buffer memory: " + to_string(r));

					//Bind memory to buffer
					binding_data.device.bindBufferMemory(buffer, memory, 0);
				}

				BufferVulkan::~BufferVulkan()
				{
					binding_data.device.destroyBuffer(buffer);
					binding_data.device.freeMemory(memory);
				}

				void BufferVulkan::copyFromData(void* pData)
				{
					void* ptr;
					binding_data.device.mapMemory(memory, 0, size, {}, &ptr);
					memcpy(ptr, pData, size);
					binding_data.device.unmapMemory(memory);
				}

				void BufferVulkan::copyFromBuffer(vk::Buffer srcBuffer)
				{
					vk::CommandBuffer cmd = CommandBuffer::begin();

					vk::BufferCopy copy = vk::BufferCopy(0, 0, size);
					cmd.copyBuffer(srcBuffer, buffer, 1, &copy);
					
					CommandBuffer::end(cmd);
				}

				std::unique_ptr<BufferVulkan> BufferVulkan::createOptimized(size_t size, void* data, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::SharingMode sharingMode)
				{
					//Vulkan's buffers will be able to enable high performance memory when we're using a staging buffer instead of directly copying data to the gpu
					BufferVulkan staging = BufferVulkan(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
					staging.copyFromData(data);

					std::unique_ptr<BufferVulkan> buffer = std::make_unique<BufferVulkan>(size, usage | vk::BufferUsageFlagBits::eTransferDst, properties, sharingMode);
					buffer->copyFromBuffer(staging.getBuffer());

					return move(buffer);
				}

				uint32_t BufferVulkan::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
				{
					vk::PhysicalDeviceMemoryProperties const memProps = Core::binding_data.physical.getMemoryProperties();
					for (uint32_t i = 0; i < memProps.memoryTypeCount; i++)
					{
						//Select a memory type that fits both our typefilter and requested property flags
						if (typeFilter & (1 << i) && (memProps.memoryTypes[i].propertyFlags & properties) == properties)
							return i;
					}

					Misc::Console::error("Failed to find a suitable memory type!");
					return 0;
				}
			}
		}
	}
}
