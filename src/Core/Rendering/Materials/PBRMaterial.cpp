#include "PBRMaterial.h"
#include <Core/BindingData.h>
#include <Core/Engine.h>


#include "Data/Mesh.h"
#include "Data/Resources.h"

namespace Tristeon::Core::Rendering
{
	REGISTER_TYPE_CPP(PBRMaterial);

	PBRMaterial::~PBRMaterial()
	{
		binding_data.device.destroyDescriptorSetLayout(_setLayout);
		binding_data.device.freeDescriptorSets(binding_data.descriptorPool, _set);
	}

	PBRMaterial::PBRMaterial(PipelineProperties pProperties): Material(pProperties) { }

	ShaderFile* PBRMaterial::shader()
	{
		static ShaderFile shader("Files/Shaders/Standard.vert.spv", "Files/Shaders/Standard.frag.spv");
		return &shader;
	}

	void PBRMaterial::createPipeline()
	{
		if ((VkPipeline)_pipeline != VK_NULL_HANDLE)
			binding_data.device.destroyPipeline(_pipeline);
		if ((VkPipelineLayout)_layout != VK_NULL_HANDLE)
			binding_data.device.destroyPipelineLayout(_layout);

		if (!_set)
			createDescriptorSets();

		auto bindings = Data::Vertex::binding();
		auto attributes = Data::Vertex::attributes();
		vk::PipelineVertexInputStateCreateInfo stageVertex{ {}, 1, &bindings, attributes.size(), attributes.data() };

		vk::PipelineInputAssemblyStateCreateInfo stageAssembly{ {}, _properties.topology, VK_FALSE };

		vk::Viewport viewport{ 0, 0, (float)binding_data.extent.width, (float)binding_data.extent.height, 0, 1.0f };
		vk::Rect2D scissor{ vk::Offset2D { 0, 0 }, binding_data.extent };
		vk::PipelineViewportStateCreateInfo stateViewport{ {}, 1, &viewport, 1, &scissor };

		vk::PipelineRasterizationStateCreateInfo stateRasterization{ {},
			_properties.depth_clamp, //Depth clamp
			VK_FALSE, //Rasterizer discard (skips rasterizer stage entirely when VK_TRUE)
			_properties.polygon_mode,
			_properties.cull_mode, _properties.front_face };

		vk::PipelineMultisampleStateCreateInfo stateMultisample{ {}, _properties.sample_count, VK_FALSE, 1.0, nullptr, VK_FALSE, VK_FALSE };

		std::array<vk::PipelineColorBlendAttachmentState, 2> stateBlendAttachments{
			vk::PipelineColorBlendAttachmentState { _properties.blend_enable,
				_properties.blend_col_src, _properties.blend_col_dst, _properties.blend_col_op,
				_properties.blend_alpha_src, _properties.blend_alpha_dst, _properties.blend_alpha_op,
				vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
			},
			vk::PipelineColorBlendAttachmentState { _properties.blend_enable,
				_properties.blend_col_src, _properties.blend_col_dst, _properties.blend_col_op,
				_properties.blend_alpha_src, _properties.blend_alpha_dst, _properties.blend_alpha_op,
				vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
			}
		};
		vk::PipelineColorBlendStateCreateInfo stateBlend{ {}, VK_FALSE, vk::LogicOp::eCopy, stateBlendAttachments.size(), stateBlendAttachments.data(), { 0, 0, 0, 0 } };

		vk::PipelineDynamicStateCreateInfo stateDynamic{ {}, 0, nullptr };

		vk::PipelineDepthStencilStateCreateInfo stateDepth{ {}, true, true, vk::CompareOp::eGreater };

		std::array<vk::DescriptorSetLayout, 2> layouts{ binding_data.transformLayout, _setLayout };
		vk::PipelineLayoutCreateInfo layoutCi{ {}, layouts };
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
			&stateDepth,
			&stateBlend,
			&stateDynamic,
			_layout,
			binding_data.offscreenPass,
			0,
			nullptr,
			0
		};

		auto r = binding_data.device.createGraphicsPipeline(nullptr, pipeline_ci);
		VULKAN_DEBUG(r.result);
		_pipeline = r.value;
	}

	void PBRMaterial::createDescriptorSets()
	{
		if ((VkDescriptorSetLayout)_setLayout != VK_NULL_HANDLE)
			binding_data.device.destroyDescriptorSetLayout(_setLayout);
		if ((VkDescriptorSet)_set != VK_NULL_HANDLE)
			binding_data.device.freeDescriptorSets(binding_data.descriptorPool, _set);

		std::array<vk::DescriptorSetLayoutBinding, 5> bindings{
			vk::DescriptorSetLayoutBinding { 0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr }, //Albedo
			vk::DescriptorSetLayoutBinding { 1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr }, //Normal
			vk::DescriptorSetLayoutBinding { 2, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr }, //Metallic
			vk::DescriptorSetLayoutBinding { 3, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr }, //Roughness
			vk::DescriptorSetLayoutBinding { 4, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr }  //AO
		};

		const vk::DescriptorSetLayoutCreateInfo layout_ci{ {}, bindings };
		_setLayout = binding_data.device.createDescriptorSetLayout(layout_ci);

		const vk::DescriptorSetAllocateInfo alloc{ binding_data.descriptorPool, 1, &_setLayout };
		_set = binding_data.device.allocateDescriptorSets(alloc)[0];

		auto albedo_info = vk::DescriptorImageInfo(albedo->sampler(), albedo->view(), vk::ImageLayout::eShaderReadOnlyOptimal);
		auto normal_info = vk::DescriptorImageInfo(normal->sampler(), normal->view(), vk::ImageLayout::eShaderReadOnlyOptimal);
		auto metallic_info = vk::DescriptorImageInfo(metallic->sampler(), metallic->view(), vk::ImageLayout::eShaderReadOnlyOptimal);
		auto roughness_info = vk::DescriptorImageInfo(roughness->sampler(), roughness->view(), vk::ImageLayout::eShaderReadOnlyOptimal);
		auto ao_info = vk::DescriptorImageInfo(ao->sampler(), ao->view(), vk::ImageLayout::eShaderReadOnlyOptimal);

		std::array<vk::WriteDescriptorSet, 5> writes{
			vk::WriteDescriptorSet{ _set, 0, 0, 1, vk::DescriptorType::eCombinedImageSampler, &albedo_info },
			vk::WriteDescriptorSet{ _set, 1, 0, 1, vk::DescriptorType::eCombinedImageSampler, &normal_info },
			vk::WriteDescriptorSet{ _set, 2, 0, 1, vk::DescriptorType::eCombinedImageSampler, &metallic_info },
			vk::WriteDescriptorSet{ _set, 3, 0, 1, vk::DescriptorType::eCombinedImageSampler, &roughness_info },
			vk::WriteDescriptorSet{ _set, 4, 0, 1, vk::DescriptorType::eCombinedImageSampler, &ao_info },
		};
		binding_data.device.updateDescriptorSets(writes, {});
	}

	nlohmann::json PBRMaterial::serialize()
	{
		nlohmann::json j = Material::serialize();
		j["typeID"] = Type<PBRMaterial>::fullName();
		j["albedo"] = albedoPath;	
		j["normal"] = normalPath;
		j["metallic"] = metallicPath;
		j["roughness"] = roughnessPath;
		j["ao"] = aoPath;
		return j;
	}

	void PBRMaterial::deserialize(nlohmann::json json)
	{
		Material::deserialize(json);

		albedoPath = json.value("albedo", std::string());
		albedo = Data::Resources::assetLoad<Data::Image>(albedoPath);
		if (!albedo) albedo = Data::Resources::assetLoad<Data::Image>("Files/Textures/white.jpg");

		normalPath = json.value("normal", std::string());
		normal = Data::Resources::assetLoad<Data::Image>(normalPath);
		if (!normal) normal = Data::Resources::assetLoad<Data::Image>("Files/Textures/white.jpg");

		metallicPath = json.value("metallic", std::string());
		metallic = Data::Resources::assetLoad<Data::Image>(metallicPath);
		if (!metallic) metallic = Data::Resources::assetLoad<Data::Image>("Files/Textures/white.jpg");
		
		roughnessPath = json.value("roughness", std::string());
		roughness = Data::Resources::assetLoad<Data::Image>(roughnessPath);
		if (!roughness) roughness = Data::Resources::assetLoad<Data::Image>("Files/Textures/white.jpg");
		
		aoPath = json.value("ao", std::string());
		ao = Data::Resources::assetLoad<Data::Image>(aoPath);
		if (!ao) ao = Data::Resources::assetLoad<Data::Image>("Files/Textures/white.jpg");
	}
}