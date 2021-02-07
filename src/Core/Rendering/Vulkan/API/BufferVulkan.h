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
				struct QueueFamilyIndices;

				class BufferVulkan
				{
				public:
					BufferVulkan(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eDeviceLocal, vk::SharingMode sharingMode = vk::SharingMode::eExclusive);
					~BufferVulkan();
					vk::Buffer getBuffer() const { return buffer; }
					vk::DeviceMemory getDeviceMemory() const { return memory; }

					void copyFromData(void* data);
					void copyFromBuffer(vk::Buffer srcBuffer);

					static std::unique_ptr<BufferVulkan> createOptimized(size_t size, void* data, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eDeviceLocal, vk::SharingMode sharingMode = vk::SharingMode::eExclusive);

					static uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
				private:
					vk::Buffer buffer;
					vk::DeviceMemory memory;
					size_t size;
				};
			}
		}
	}
}
