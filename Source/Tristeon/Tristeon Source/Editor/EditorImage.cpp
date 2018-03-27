#include "EditorImage.h"
#include "Core/Rendering/Vulkan/HelperClasses/VulkanImage.h"
#include "Misc/Console.h"
#include "Core/Rendering/Vulkan/MaterialVulkan.h"
#include "Core/BindingData.h"
#include "Core/Rendering/Vulkan/API/BufferVulkan.h"

namespace Tristeon
{
	namespace Editor
	{
		EditorImage::EditorImage(Data::Image image, Core::BindingData* data) : image(image), binding(dynamic_cast<Core::VulkanBindingData*>(data))
		{
			//Init all resources
			createTextureImage();
			createTextureImageView();
			createTextureSampler();
			createDescriptorSets();
		}

		EditorImage::EditorImage(std::string filePath, Core::BindingData* data) : EditorImage(Data::Image(filePath), data)
		{
			//Empty
		}

		EditorImage::~EditorImage()
		{
			//Cleanup
			binding->device.destroySampler(sampler);
			binding->device.destroyImageView(view);
			binding->device.destroyImage(img);
			binding->device.freeMemory(mem);
		}

		ImTextureID EditorImage::getTextureID() const
		{
			return (ImTextureID)(VkDescriptorSet)set;
		}

		void EditorImage::createTextureImage()
		{
			//Get image size and data
			auto const pixels = image.getPixels();
			vk::DeviceSize const size = image.getWidth() * image.getHeight() * 4;

			//Create staging buffer
			Core::Rendering::Vulkan::BufferVulkan staging = Core::Rendering::Vulkan::BufferVulkan(binding, size, vk::BufferUsageFlagBits::eTransferSrc, 
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
			staging.copyFromData(pixels);

			//Create vulkan image
			Core::Rendering::Vulkan::VulkanImage::createImage(
				binding,
				image.getWidth(), image.getHeight(),
				vk::Format::eR8G8B8A8Unorm,
				vk::ImageTiling::eOptimal,
				vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
				vk::MemoryPropertyFlagBits::eDeviceLocal,
				img, mem);

			//Change texture format to transfer destination
			Core::Rendering::Vulkan::VulkanImage::transitionImageLayout(binding, img, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
			//Send data from our staging buffer to our image
			Core::Rendering::Vulkan::VulkanImage::copyBufferToImage(binding, staging.getBuffer(), img, image.getWidth(), image.getHeight());
			//Change texture format to shader read only
			Core::Rendering::Vulkan::VulkanImage::transitionImageLayout(binding, img, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
		}

		void EditorImage::createTextureImageView()
		{
			//Create image view for diffuse texture
			view = Core::Rendering::Vulkan::VulkanImage::createImageView(binding->device, img, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor);
		}

		void EditorImage::createTextureSampler()
		{
			vk::SamplerCreateInfo ci = vk::SamplerCreateInfo({},
				vk::Filter::eLinear, vk::Filter::eLinear,
				vk::SamplerMipmapMode::eLinear,
				vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat,
				0, VK_TRUE, 16, VK_FALSE,
				vk::CompareOp::eAlways,
				0, 0,
				vk::BorderColor::eIntOpaqueBlack,
				VK_FALSE);

			vk::Result const r = binding->device.createSampler(&ci, nullptr, &sampler);
			Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to create image sampler: " + to_string(r));
		}

		void EditorImage::createDescriptorSets()
		{
			//Create descriptor set layout (temporary)
			vk::DescriptorSetLayoutBinding b = vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr);
			vk::DescriptorSetLayoutCreateInfo const ci = vk::DescriptorSetLayoutCreateInfo({}, 1, &b);
			vk::DescriptorSetLayout layout = binding->device.createDescriptorSetLayout(ci);

			//Allocate the descriptor set
			vk::DescriptorSetAllocateInfo alloc_info = vk::DescriptorSetAllocateInfo(
				binding->descriptorPool,
				1,
				&layout);
			binding->device.allocateDescriptorSets(&alloc_info, &set);

			//Update the Descriptor Set
			vk::DescriptorImageInfo image = vk::DescriptorImageInfo(sampler, view, vk::ImageLayout::eShaderReadOnlyOptimal);
			vk::WriteDescriptorSet samplerWrite = vk::WriteDescriptorSet(set, 0, 0, 1, vk::DescriptorType::eCombinedImageSampler, &image, nullptr, nullptr);

			binding->device.updateDescriptorSets(1, &samplerWrite, 0, nullptr);
			binding->device.destroyDescriptorSetLayout(layout);
		}
	}
}
