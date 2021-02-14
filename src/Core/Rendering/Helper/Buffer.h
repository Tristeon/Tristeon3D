#pragma once
#include <vulkan/vulkan.hpp>

namespace Tristeon::Core::Rendering
{
	class Buffer
	{
	public:
		Buffer(const size_t& size, const vk::BufferUsageFlags& usage, const vk::MemoryPropertyFlags& memProperties);
		~Buffer();
		void copyFromData(void* data, const uint32_t& size, const uint32_t& offset = 0) const;
		void copyFromBuffer(const vk::Buffer& src, const uint32_t& size, const uint32_t& srcOffset = 0, const uint32_t& dstOffset = 0) const;

		static std::unique_ptr<Buffer> createOptimized(void* data, const uint32_t& size, const vk::BufferUsageFlags& usage, const vk::MemoryPropertyFlags& memProperties);
		vk::Buffer buffer = nullptr;
		vk::DeviceMemory memory = nullptr;
	};
}