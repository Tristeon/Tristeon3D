#include "ScreenMaterial.h"
#include "Core/Engine.h"

namespace Tristeon::Core::Rendering
{
	REGISTER_TYPE_CPP(ScreenMaterial);

	ScreenMaterial::~ScreenMaterial()
	{
		binding_data.device.destroyDescriptorSetLayout(_setLayout);
		binding_data.device.freeDescriptorSets(binding_data.descriptorPool, _set);
	}

	ShaderFile* ScreenMaterial::shader()
	{
		static ShaderFile shader("Files/Shaders/Screen.vert.spv", "Files/Shaders/Screen.frag.spv");
		return &shader;
	}

	void ScreenMaterial::createPipeline()
	{
		if ((VkPipeline)_pipeline != VK_NULL_HANDLE)
			binding_data.device.destroyPipeline(_pipeline);
		if ((VkPipelineLayout)_layout != VK_NULL_HANDLE)
			binding_data.device.destroyPipelineLayout(_layout);

		auto bindings = std::array<vk::VertexInputBindingDescription, 0>{};// Data::Vertex::binding();
		auto attributes = std::array<vk::VertexInputAttributeDescription, 0>{};// Data::Vertex::attributes();
		vk::PipelineVertexInputStateCreateInfo stageVertex{ {}, bindings.size(), bindings.data(), attributes.size(), attributes.data() };

		vk::PipelineInputAssemblyStateCreateInfo stageAssembly{ {}, _properties.topology, VK_FALSE };

		vk::Viewport viewport{ 0, 0, (float)binding_data.extent.width, (float)binding_data.extent.height, 0, 1.0f };
		vk::Rect2D scissor{ vk::Offset2D { 0, 0 }, binding_data.extent };
		vk::PipelineViewportStateCreateInfo stateViewport{ {}, 1, &viewport, 1, &scissor };

		vk::PipelineRasterizationStateCreateInfo stateRasterization{ {},
			_properties.depth_clamp,
			VK_FALSE, //Rasterizer discard (skips rasterizer stage entirely when VK_TRUE)
			_properties.polygon_mode,
			_properties.cull_mode, _properties.front_face };

		vk::PipelineMultisampleStateCreateInfo stateMultisample{ {}, _properties.sample_count, VK_FALSE, 1.0, nullptr, VK_FALSE, VK_FALSE };

		vk::PipelineColorBlendAttachmentState stateBlendAttachment{ _properties.blend_enable,
			_properties.blend_col_src, _properties.blend_col_dst, _properties.blend_col_op,
			_properties.blend_alpha_src, _properties.blend_alpha_dst, _properties.blend_alpha_op,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA };
		vk::PipelineColorBlendStateCreateInfo stateBlend{ {}, VK_FALSE, vk::LogicOp::eCopy, 1, &stateBlendAttachment, { 0, 0, 0, 0 } };

		vk::PipelineDynamicStateCreateInfo stateDynamic{ {}, 0, nullptr };

		vk::PipelineLayoutCreateInfo layoutCi{ {}, 1, &_setLayout};
		_layout = binding_data.device.createPipelineLayout(layoutCi);

		if (shader()->stages().empty())
			throw std::runtime_error("Can't use a shader with 0 shaders");

		auto stages = shader()->stages();

		vk::GraphicsPipelineCreateInfo pipeline_ci{ {},
			(uint32_t)stages.size(),
			stages.data(),
			&stageVertex,
			&stageAssembly,
			nullptr,
			&stateViewport,
			&stateRasterization,
			&stateMultisample,
			nullptr,
			&stateBlend,
			&stateDynamic,
			_layout,
			binding_data.outputPass,
			0,
			nullptr,
			0
		};

		auto r = binding_data.device.createGraphicsPipeline(nullptr, pipeline_ci);
		VULKAN_DEBUG(r.result);
		_pipeline = r.value;
	}

	void ScreenMaterial::createDescriptorSets()
	{
		if ((VkDescriptorSetLayout)_setLayout != VK_NULL_HANDLE)
			binding_data.device.destroyDescriptorSetLayout(_setLayout);
		if ((VkDescriptorSet)_set != VK_NULL_HANDLE)
			binding_data.device.freeDescriptorSets(binding_data.descriptorPool, _set);
		
		std::array<vk::DescriptorSetLayoutBinding, 3> bindings{
			vk::DescriptorSetLayoutBinding { 0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr },
			vk::DescriptorSetLayoutBinding { 1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr },
			vk::DescriptorSetLayoutBinding { 2, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr }
		};

		const vk::DescriptorSetLayoutCreateInfo layout_ci{ {}, bindings };
		_setLayout = binding_data.device.createDescriptorSetLayout(layout_ci);

		const vk::DescriptorSetAllocateInfo alloc{ binding_data.descriptorPool, 1, &_setLayout };
		_set = binding_data.device.allocateDescriptorSets(alloc)[0];

		auto color_info = vk::DescriptorImageInfo(binding_data.offscreenColor.sampler, binding_data.offscreenColor.view, vk::ImageLayout::eShaderReadOnlyOptimal);
		auto depth_info = vk::DescriptorImageInfo(binding_data.offscreenDepth.sampler, binding_data.offscreenDepth.view, vk::ImageLayout::eShaderReadOnlyOptimal);
		auto normal_info = vk::DescriptorImageInfo(binding_data.offscreenNormal.sampler, binding_data.offscreenNormal.view, vk::ImageLayout::eShaderReadOnlyOptimal);

		std::array<vk::WriteDescriptorSet, 3> writes{
			vk::WriteDescriptorSet{ _set, 0, 0, 1, vk::DescriptorType::eCombinedImageSampler, &color_info },
			vk::WriteDescriptorSet{ _set, 1, 0, 1, vk::DescriptorType::eCombinedImageSampler, &depth_info },
			vk::WriteDescriptorSet{ _set, 2, 0, 1, vk::DescriptorType::eCombinedImageSampler, &normal_info },
		};
		binding_data.device.updateDescriptorSets(writes, nullptr);
	}
}