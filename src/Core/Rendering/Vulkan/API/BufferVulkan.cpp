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
				BufferVulkan::BufferVulkan(vk::Device device, vk::PhysicalDevice gpu, vk::SurfaceKHR surface, size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::SharingMode sharingMode) : size(size), device(device)
				{
					//Create a vulkan buffer with our given settings
					QueueFamilyIndices const indices = QueueFamilyIndices::get(gpu, surface);
					uint32_t families[] = { indices.graphicsFamily, indices.presentFamily };

					vk::BufferCreateInfo ci = vk::BufferCreateInfo({}, size, usage, sharingMode, 2, families);
					vk::Result r = device.createBuffer(&ci, nullptr, &buffer);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to create buffer: " + to_string(r));

					//Allocate VRAM accordingly
					vk::MemoryRequirements const memReq = device.getBufferMemoryRequirements(buffer);
					uint32_t const memType = findMemoryType(gpu, memReq.memoryTypeBits, properties);
					vk::MemoryAllocateInfo mallocInfo = vk::MemoryAllocateInfo(memReq.size, memType);
						
					r = device.allocateMemory(&mallocInfo, nullptr, &memory);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to allocate buffer memory: " + to_string(r));

					//Bind memory to buffer
					device.bindBufferMemory(buffer, memory, 0);
				}

				BufferVulkan::BufferVulkan(size_t size, vk::BufferUsageFlags usage,
					vk::MemoryPropertyFlags properties, vk::SharingMode sharingMode) : BufferVulkan(VulkanBindingData::getInstance()->device, VulkanBindingData::getInstance()->physicalDevice, VulkanBindingData::getInstance()->swapchain->getSurface(), size, usage, properties, sharingMode)
				{
					//Empty
				}

				BufferVulkan::~BufferVulkan()
				{
					device.destroyBuffer(buffer);
					device.freeMemory(memory);
				}

				void BufferVulkan::copyFromData(void* pData)
				{
					void* ptr;
					device.mapMemory(memory, 0, size, {}, &ptr);
					memcpy(ptr, pData, size);
					device.unmapMemory(memory);
				}

				void BufferVulkan::copyFromBuffer(vk::Buffer srcBuffer, vk::CommandPool cmdPool, vk::Queue graphicsQueue)
				{
					vk::CommandBuffer cmd = CommandBuffer::begin(cmdPool, device);

					vk::BufferCopy copy = vk::BufferCopy(0, 0, size);
					cmd.copyBuffer(srcBuffer, buffer, 1, &copy);
					
					CommandBuffer::end(cmd, graphicsQueue, device, cmdPool);
				}

				std::unique_ptr<BufferVulkan> BufferVulkan::createOptimized(vk::Device device, vk::PhysicalDevice gpu, vk::SurfaceKHR surface, vk::CommandPool cmdPool,
					vk::Queue graphicsQueue, size_t size, void* data, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::SharingMode sharingMode)
				{
					//Vulkan's buffers will be able to enable high performance memory when we're using a staging buffer instead of directly copying data to the gpu
					BufferVulkan staging = BufferVulkan(device, gpu, surface, size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
					staging.copyFromData(data);

					std::unique_ptr<BufferVulkan> buffer = std::make_unique<BufferVulkan>(device, gpu, surface, size, usage | vk::BufferUsageFlagBits::eTransferDst, properties, sharingMode);
					buffer->copyFromBuffer(staging.getBuffer(), cmdPool, graphicsQueue);

					return move(buffer);
				}

				std::unique_ptr<BufferVulkan> BufferVulkan::createOptimized(size_t size, void* data,
					vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::SharingMode sharingMode)
				{
					VulkanBindingData* bindingData = VulkanBindingData::getInstance();
					return move(createOptimized(bindingData->device, bindingData->physicalDevice, bindingData->swapchain->getSurface(), 
						bindingData->commandPool, bindingData->graphicsQueue, 
						size, data, usage, properties, sharingMode));
				}

				uint32_t BufferVulkan::findMemoryType(vk::PhysicalDevice gpu, uint32_t typeFilter, vk::MemoryPropertyFlags properties)
				{
					vk::PhysicalDeviceMemoryProperties const memProps = gpu.getMemoryProperties();
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
