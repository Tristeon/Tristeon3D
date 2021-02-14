#include "Buffer.h"

#include <Core/BindingData.h>
#include <Core/Engine.h>
#include <Core/Rendering/Helper/DeviceSelector.h>
#include <Core/Rendering/Helper/OneTimeCmd.h>

namespace Tristeon::Core::Rendering
{
	Buffer::Buffer(const size_t& size, const vk::BufferUsageFlags& usage, const vk::MemoryPropertyFlags& memProperties)
	{
		const vk::BufferCreateInfo buf_ci{ {}, size, usage, vk::SharingMode::eExclusive };
		buffer = binding_data.device.createBuffer(buf_ci);

		const auto req = binding_data.device.getBufferMemoryRequirements(buffer);
		const vk::MemoryAllocateInfo mem_ci{ req.size, DeviceSelector::getMemoryType(req.memoryTypeBits, memProperties) };
		memory = binding_data.device.allocateMemory(mem_ci);

		binding_data.device.bindBufferMemory(buffer, memory, 0);
	}

	Buffer::~Buffer()
	{
		if ((VkDeviceMemory)memory != VK_NULL_HANDLE)
			binding_data.device.freeMemory(memory);
		if ((VkBuffer)buffer != VK_NULL_HANDLE)
			binding_data.device.destroyBuffer(buffer);
	}

	void Buffer::copyFromData(void* data, const uint32_t& size, const uint32_t& offset) const
	{
		void* ptr = binding_data.device.mapMemory(memory, offset, size, {});
		memcpy(ptr, data, size);
		binding_data.device.unmapMemory(memory);
	}

	void Buffer::copyFromBuffer(const vk::Buffer& src, const uint32_t& size, const uint32_t& srcOffset, const uint32_t& dstOffset) const
	{
		const auto cmd = OneTimeCmd::begin(binding_data.transferPool);

		auto copy = vk::BufferCopy{ srcOffset, dstOffset, size };
		cmd.copyBuffer(src, buffer, 1, &copy);
		OneTimeCmd::end(cmd, binding_data.transferQueue, binding_data.transferPool);
	}

	std::unique_ptr<Buffer> Buffer::createOptimized(void* data, const uint32_t& size, const vk::BufferUsageFlags& usage, const vk::MemoryPropertyFlags& memProperties)
	{
		const Buffer staging = Buffer(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
		staging.copyFromData(data, size);

		auto buffer = std::make_unique<Buffer>(size, usage | vk::BufferUsageFlagBits::eTransferDst, memProperties);
		buffer->copyFromBuffer(staging.buffer, size);

		return std::move(buffer);
	}
}