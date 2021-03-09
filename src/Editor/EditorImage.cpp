#ifdef TRISTEON_EDITOR
#include "EditorImage.h"
#include <Data/Resources.h>
#include <Core/Rendering/Helper/Buffer.h>
#include <Core/BindingData.h>

namespace Tristeon::Editor
{
	EditorImage::EditorImage()
	{
		//Empty
	}

	EditorImage::EditorImage(Data::Image* pImage) : image(pImage)
	{
		//Init all resources
		createDescriptorSets();
	}

	EditorImage::EditorImage(const std::string& pFilePath) : EditorImage(Data::Resources::assetLoad<Data::Image>(pFilePath))
	{
		//Empty
	}

	EditorImage::~EditorImage()
	{
	}

	ImTextureID EditorImage::getTextureID() const
	{
		return (ImTextureID)(VkDescriptorSet)set;
	}

	void EditorImage::createDescriptorSets()
	{
		//Create descriptor set layout (temporary)
		auto b = vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr);
		auto const ci = vk::DescriptorSetLayoutCreateInfo({}, 1, &b);
		auto layout = Core::binding_data.device.createDescriptorSetLayout(ci);

		//Allocate the descriptor set
		const auto allocInfo = vk::DescriptorSetAllocateInfo(Core::binding_data.descriptorPool, layout);
		set = Core::binding_data.device.allocateDescriptorSets(allocInfo)[0];

		//Update the Descriptor Set
		auto imageInfo = vk::DescriptorImageInfo(image->sampler(), image->view(), vk::ImageLayout::eShaderReadOnlyOptimal);
		auto samplerWrite = vk::WriteDescriptorSet(set, 0, 0, 1, vk::DescriptorType::eCombinedImageSampler, &imageInfo, nullptr, nullptr);

		Core::binding_data.device.updateDescriptorSets(1, &samplerWrite, 0, nullptr);
		Core::binding_data.device.destroyDescriptorSetLayout(layout);
	}
}

#endif