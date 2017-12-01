#pragma once
#include <vulkan/vulkan.hpp>
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
				 * \brief Helper class for creating/copying buffers
				 */
				class VulkanBuffer
				{
				public:
					/**
					 * \brief Creates a vk::Buffer and allocates memory based on the given size
					 * \param vk Rendering data
					 * \param size The size of the allocated buffer memory
					 * \param usage The intended usage of the buffer
					 * \param properties Specifices memory type properties
					 * \param buffer The commandbuffer
					 * \param bufferMemory The buffer memory
					 */
					static void createBuffer(VulkanBindingData* vk, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory);
					/**
					 * \brief Tries to find a memory type fitting the given typefilter
					 * \param vk Rendering data
					 * \param typeFilter The typefilter that the memory types should be checked against
					 * \param properties Specifies the requested memory type properties
					 * \return Returns the selected memory type
					 */
					static uint32_t findMemoryType(VulkanBindingData* vk, uint32_t typeFilter, vk::MemoryPropertyFlags properties);
					/**
					 * \brief Copies data from one buffer into another
					 * \param vk Rendering data
					 * \param srcBuffer The buffer that data should be copied from
					 * \param dstBuffer The buffer that data should be copied to
					 * \param size The size of the data
					 */
					static void copyBuffer(VulkanBindingData* vk, vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);
				};
			}
		}
	}
}