#include "PBRMaterial.h"
#include <Core/RenderData.h>
#include <Core/Engine.h>

#include "Data/Mesh.h"
#include "Data/Resources.h"

namespace Tristeon::Core::Rendering
{
	REGISTER_TYPE_CPP(PBRMaterial);

	PBRMaterial::PBRMaterial(PipelineProperties pProperties) : Material(pProperties) { }

	ShaderFile* PBRMaterial::shader()
	{
		static ShaderFile shader("Files/Shaders/Standard.vert.spv", "Files/Shaders/Standard.frag.spv");
		return &shader;
	}

	void PBRMaterial::createPipeline()
	{
		if ((VkPipeline)_pipeline != VK_NULL_HANDLE)
			renderData.device.destroyPipeline(_pipeline);
		if ((VkPipelineLayout)_layout != VK_NULL_HANDLE)
			renderData.device.destroyPipelineLayout(_layout);

		if (!_sets[0]) //any set will do
			createDescriptorSets();

		auto bindings = Data::Vertex::binding();
		auto attributes = Data::Vertex::attributes();
		vk::PipelineVertexInputStateCreateInfo stageVertex{ {}, 1, &bindings, attributes.size(), attributes.data() };

		vk::PipelineInputAssemblyStateCreateInfo stageAssembly{ {}, _properties.topology, VK_FALSE };

		vk::Viewport viewport{ 0, 0, (float)renderData.extent.width, (float)renderData.extent.height, 0, 1.0f };
		vk::Rect2D scissor{ vk::Offset2D { 0, 0 }, renderData.extent };
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

		std::array<vk::DescriptorSetLayout, 2> layouts{ renderData.transformLayout, _setLayout };
		vk::PipelineLayoutCreateInfo layoutCi{ {}, layouts };
		_layout = renderData.device.createPipelineLayout(layoutCi);

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
			renderData.offscreenPass,
			0,
			nullptr,
			0
		};

		auto r = renderData.device.createGraphicsPipeline(nullptr, pipeline_ci);
		VULKAN_DEBUG(r.result);
		_pipeline = r.value;
	}

	void PBRMaterial::createDescriptorSets()
	{
		if ((VkDescriptorSetLayout)_setLayout != VK_NULL_HANDLE)
			renderData.device.destroyDescriptorSetLayout(_setLayout);

		std::array<vk::DescriptorSetLayoutBinding, 5> bindings{
			vk::DescriptorSetLayoutBinding { 0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr }, //Albedo
			vk::DescriptorSetLayoutBinding { 1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr }, //Normal
			vk::DescriptorSetLayoutBinding { 2, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr }, //Metallic
			vk::DescriptorSetLayoutBinding { 3, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr }, //Roughness
			vk::DescriptorSetLayoutBinding { 4, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr }  //AO
		};

		const vk::DescriptorSetLayoutCreateInfo layout_ci{ {}, bindings };
		_setLayout = renderData.device.createDescriptorSetLayout(layout_ci);
		
		for (uint8_t i = 0; i < RenderData::IMAGES_IN_FLIGHT; i++)
		{
			if ((VkDescriptorSet)_sets[i] != VK_NULL_HANDLE)
				renderData.device.freeDescriptorSets(renderData.descriptorPool, _sets[i]);

			const vk::DescriptorSetAllocateInfo alloc{ renderData.descriptorPool, 1, &_setLayout };
			_sets[i] = renderData.device.allocateDescriptorSets(alloc)[0];

			auto albedo_info = vk::DescriptorImageInfo(_albedo->sampler(), _albedo->view(), vk::ImageLayout::eShaderReadOnlyOptimal);
			auto normal_info = vk::DescriptorImageInfo(_normal->sampler(), _normal->view(), vk::ImageLayout::eShaderReadOnlyOptimal);
			auto metallic_info = vk::DescriptorImageInfo(_metallic->sampler(), _metallic->view(), vk::ImageLayout::eShaderReadOnlyOptimal);
			auto roughness_info = vk::DescriptorImageInfo(_roughness->sampler(), _roughness->view(), vk::ImageLayout::eShaderReadOnlyOptimal);
			auto ao_info = vk::DescriptorImageInfo(_ao->sampler(), _ao->view(), vk::ImageLayout::eShaderReadOnlyOptimal);

			std::array<vk::WriteDescriptorSet, 5> writes{
				vk::WriteDescriptorSet{ _sets[i], 0, 0, 1, vk::DescriptorType::eCombinedImageSampler, &albedo_info },
				vk::WriteDescriptorSet{ _sets[i], 1, 0, 1, vk::DescriptorType::eCombinedImageSampler, &normal_info },
				vk::WriteDescriptorSet{ _sets[i], 2, 0, 1, vk::DescriptorType::eCombinedImageSampler, &metallic_info },
				vk::WriteDescriptorSet{ _sets[i], 3, 0, 1, vk::DescriptorType::eCombinedImageSampler, &roughness_info },
				vk::WriteDescriptorSet{ _sets[i], 4, 0, 1, vk::DescriptorType::eCombinedImageSampler, &ao_info },
			};
			renderData.device.updateDescriptorSets(writes, {});
		}
	}

	Data::Image* PBRMaterial::texture(TextureType type) const
	{
		switch (type)
		{
		case TextureType::Albedo:
			return _albedo;
		case TextureType::Normal:
			return _normal;
		case TextureType::Metallic:
			return _metallic;
		case TextureType::Roughness:
			return _roughness;
		case TextureType::Ao:
			return _ao;
		case TextureType::EndOfEnum:
			return nullptr;
		}
		return nullptr;
	}

	nlohmann::json PBRMaterial::serialize()
	{
		nlohmann::json j = Material::serialize();
		j["typeID"] = Type<PBRMaterial>::fullName();
		j["albedo"] = _albedoPath;
		j["normal"] = _normalPath;
		j["metallic"] = _metallicPath;
		j["roughness"] = _roughnessPath;
		j["ao"] = _aoPath;
		return j;
	}

	void PBRMaterial::deserialize(nlohmann::json json)
	{
		Material::deserialize(json);

		_albedoPath = json.value("albedo", std::string());
		_albedo = Data::Resources::assetLoad<Data::Image>(_albedoPath);
		if (!_albedo) _albedo = Data::Resources::assetLoad<Data::Image>("Files/Textures/white.jpg");

		_normalPath = json.value("normal", std::string());
		_normal = Data::Resources::assetLoad<Data::Image>(_normalPath);
		if (!_normal) _normal = Data::Resources::assetLoad<Data::Image>("Files/Textures/white.jpg");

		_metallicPath = json.value("metallic", std::string());
		_metallic = Data::Resources::assetLoad<Data::Image>(_metallicPath);
		if (!_metallic) _metallic = Data::Resources::assetLoad<Data::Image>("Files/Textures/white.jpg");

		_roughnessPath = json.value("roughness", std::string());
		_roughness = Data::Resources::assetLoad<Data::Image>(_roughnessPath);
		if (!_roughness) _roughness = Data::Resources::assetLoad<Data::Image>("Files/Textures/white.jpg");

		_aoPath = json.value("ao", std::string());
		_ao = Data::Resources::assetLoad<Data::Image>(_aoPath);
		if (!_ao) _ao = Data::Resources::assetLoad<Data::Image>("Files/Textures/white.jpg");
	}
}