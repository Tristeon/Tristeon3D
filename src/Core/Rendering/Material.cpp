#include "Material.h"
#include <filesystem>

#include "Core/BindingData.h"
#include "Core/Engine.h"
#include "Data/Mesh.h"

namespace Tristeon::Core::Rendering
{
	REGISTER_TYPE_CPP(Material);
	
	PipelineProperties::PipelineProperties()
	{
		topology = vk::PrimitiveTopology::eTriangleList;

		depth_clamp = false;
		polygon_mode = vk::PolygonMode::eFill;
		cull_mode = vk::CullModeFlagBits::eBack;
		front_face = vk::FrontFace::eClockwise;

		sample_count = vk::SampleCountFlagBits::e1;

		setTransparency(false);
	}

	void PipelineProperties::setTransparency(const bool& enabled)
	{
		blend_enable = enabled;

		if (enabled)
		{
			blend_col_src = vk::BlendFactor::eSrcAlpha;
			blend_col_dst = vk::BlendFactor::eOneMinusSrcAlpha;
			blend_col_op = vk::BlendOp::eAdd;

			blend_alpha_src = vk::BlendFactor::eOne;
			blend_alpha_dst = vk::BlendFactor::eZero;
			blend_alpha_op = vk::BlendOp::eAdd;
		}
		else
		{
			blend_col_src = vk::BlendFactor::eOne;
			blend_col_dst = vk::BlendFactor::eZero;
			blend_col_op = vk::BlendOp::eAdd;

			blend_alpha_src = vk::BlendFactor::eOne;
			blend_alpha_dst = vk::BlendFactor::eZero;
			blend_alpha_op = vk::BlendOp::eAdd;
		}
	}

	Material::Material()
	{
		Collector<Material>::add(this);
	}

	Material::Material(PipelineProperties properties) : _properties(properties)
	{
		Collector<Material>::add(this);
	}

	Material::~Material()
	{
		Collector<Material>::remove(this);
		
		if ((VkPipeline)_pipeline != VK_NULL_HANDLE)
			binding_data.device.destroyPipeline(_pipeline);
		if ((VkPipelineLayout)_layout != VK_NULL_HANDLE)
			binding_data.device.destroyPipelineLayout(_layout);
	}

	void Material::buildPipeline()
	{
		if ((VkPipeline)_pipeline != VK_NULL_HANDLE)
			binding_data.device.destroyPipeline(_pipeline);
		if ((VkPipelineLayout)_layout != VK_NULL_HANDLE)
			binding_data.device.destroyPipelineLayout(_layout);

		auto bindings = Data::Vertex::binding();
		auto attributes = Data::Vertex::attributes();
		vk::PipelineVertexInputStateCreateInfo stage_vertex{ {}, 1, &bindings, attributes.size(), attributes.data() };

		vk::PipelineInputAssemblyStateCreateInfo stage_assembly{ {}, _properties.topology, VK_FALSE };

		vk::Viewport viewport{ 0, 0, (float)binding_data.extent.width, (float)binding_data.extent.height, 0, 1.0f };
		vk::Rect2D scissor{ vk::Offset2D { 0, 0 }, binding_data.extent };
		vk::PipelineViewportStateCreateInfo state_viewport{ {}, 1, &viewport, 1, &scissor };

		vk::PipelineRasterizationStateCreateInfo state_rasterization{ {},
			_properties.depth_clamp, //Depth clamp
			VK_FALSE, //Rasterizer discard (skips rasterizer stage entirely when VK_TRUE)
			_properties.polygon_mode,
			_properties.cull_mode, _properties.front_face };

		vk::PipelineMultisampleStateCreateInfo state_multisample{ {}, _properties.sample_count, VK_FALSE, 1.0, nullptr, VK_FALSE, VK_FALSE };

		vk::PipelineColorBlendAttachmentState state_blend_attachment{ _properties.blend_enable,
			_properties.blend_col_src, _properties.blend_col_dst, _properties.blend_col_op,
			_properties.blend_alpha_src, _properties.blend_alpha_dst, _properties.blend_alpha_op,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA };
		vk::PipelineColorBlendStateCreateInfo state_blend{ {}, VK_FALSE, vk::LogicOp::eCopy, 1, &state_blend_attachment, { 0, 0, 0, 0 } };

		vk::PipelineDynamicStateCreateInfo state_dynamic{ {}, 0, nullptr };

		vk::PipelineLayoutCreateInfo layout_ci{};
		_layout = binding_data.device.createPipelineLayout(layout_ci);

		if (shader()->stages().empty())
			throw std::runtime_error("Can't use a shader with 0 shaders");

		auto stages = shader()->stages();
		
		vk::GraphicsPipelineCreateInfo pipeline_ci{ {},
			(uint32_t)stages.size(),
			stages.data(),
			&stage_vertex,
			&stage_assembly,
			nullptr,
			&state_viewport,
			&state_rasterization,
			&state_multisample,
			nullptr,
			&state_blend,
			&state_dynamic,
			_layout,
			binding_data.outputPass, //TODO: Replace with appropriate render pass
			0,
			nullptr,
			0
		};

		auto r = binding_data.device.createGraphicsPipeline(nullptr, pipeline_ci);
		VULKAN_DEBUG(r.result, ":(");
		_pipeline = r.value;
	}

	ShaderFile* Material::shader()
	{
		//TODO: Remove this
		static ShaderFile file("Files/Shaders/Triangle.vert.spv", "Files/Shaders/Triangle.frag.spv");
		return &file;
	}
}