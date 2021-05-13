#include "Material.h"

#include "Core/RenderData.h"
#include "Core/Engine.h"

namespace Tristeon::Core::Rendering
{
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

		renderData.device.destroyPipeline(_pipeline);
		renderData.device.destroyPipelineLayout(_layout);

		renderData.device.destroyDescriptorSetLayout(_setLayout);

		for (auto set : _sets)
			renderData.device.freeDescriptorSets(renderData.descriptorPool, set);
	}
}
