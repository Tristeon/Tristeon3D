#include "VulkanBuffer.h"
#include "CommandBuffer.h"
#include "Misc/Console.h"

#include <Core/Rendering/Vulkan/HelperClasses/Device.h>
#include "Core/BindingData.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				void VulkanBuffer::createBuffer(VulkanBindingData* vk, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory)
				{
					//Get queue families
					QueueFamilyIndices const fam = QueueFamilyIndices::get(vk->physicalDevice, vk->swapchain->getSurface());
					uint32_t families[] = { fam.graphicsFamily, fam.presentFamily };

					//Create a buffer with the given size and usage, and with our selected families
					vk::BufferCreateInfo ci = vk::BufferCreateInfo({},
						size, usage, vk::SharingMode::eExclusive,
						2, families);
					vk::Result r = vk->device.createBuffer(&ci, nullptr, &buffer);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to create vertex buffer: " + to_string(r));

					//Find the memory requirements and the corresponding memory type. Allocate memory based on that
					vk::MemoryRequirements const memReq = vk->device.getBufferMemoryRequirements(buffer);
					vk::MemoryAllocateInfo mallocInfo = vk::MemoryAllocateInfo(memReq.size,
						findMemoryType(vk, memReq.memoryTypeBits, properties));
					r = vk->device.allocateMemory(&mallocInfo, nullptr, &bufferMemory);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to allocate vertex buffer memory: " + to_string(r));

					//Bind the buffer and the memory together
					vk->device.bindBufferMemory(buffer, bufferMemory, 0);
				}

				void VulkanBuffer::copyBuffer(VulkanBindingData* vk, vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
				{
					//Get one time submit command buffer
					vk::CommandBuffer cmd = CommandBuffer::begin(vk->commandPool, vk->device);

					//Copy
					vk::BufferCopy copyR = vk::BufferCopy(0, 0, size);
					cmd.copyBuffer(srcBuffer, dstBuffer, 1, &copyR);

					//Submit command buffer
					CommandBuffer::end(cmd, vk->graphicsQueue, vk->device, vk->commandPool);
				}

				uint32_t VulkanBuffer::findMemoryType(VulkanBindingData* vk, uint32_t typeFilter, vk::MemoryPropertyFlags properties)
				{
					//Get properties
					vk::PhysicalDeviceMemoryProperties const memProps = vk->physicalDevice.getMemoryProperties();

					//Loop overr the available memory types
					for (uint32_t i = 0; i < memProps.memoryTypeCount; i++)
					{
						//Check if the corresponding bit is set to 1 and if the propertyflags contain our required properties
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
