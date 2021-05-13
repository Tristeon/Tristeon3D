#pragma once
#include <Core/TObject.h>
#include "ShaderFile.h"
#include <vulkan/vulkan.hpp>

#include "Core/RenderData.h"

namespace Tristeon::Core::Rendering
{
	struct PipelineProperties
	{
		vk::PrimitiveTopology topology;

		bool depth_clamp;
		vk::PolygonMode polygon_mode;
		vk::CullModeFlags cull_mode;
		vk::FrontFace front_face;

		vk::SampleCountFlagBits sample_count;

		bool blend_enable{};

		vk::BlendFactor blend_col_src;
		vk::BlendFactor blend_col_dst;
		vk::BlendOp blend_col_op;

		vk::BlendFactor blend_alpha_src;
		vk::BlendFactor blend_alpha_dst;
		vk::BlendOp blend_alpha_op;

		PipelineProperties();
		void setTransparency(const bool& enabled);
	};
	
	/**
	 * \brief The material controls the appearance of renderers in the world
	 */
	class Material : public TObject
	{
		friend class RenderManager;
	public:
		Material();
		explicit Material(PipelineProperties properties);
		virtual ~Material() override;

		[[nodiscard]] vk::Pipeline pipeline()
		{
			if (!_pipeline)
				createPipeline();
			return _pipeline;
		}

		[[nodiscard]] vk::PipelineLayout layout()
		{
			if (!_layout)
				createPipeline();
			return _layout;
		}

		[[nodiscard]] vk::DescriptorSet set(const uint8_t& frameIndex)
		{
			if (!_sets[frameIndex])
				createDescriptorSets();
			return _sets[frameIndex];
		}

		virtual void createPipeline() = 0;
		virtual void createDescriptorSets() = 0;
		virtual ShaderFile* shader() = 0;
	protected:
		PipelineProperties _properties{};
		vk::PipelineLayout _layout = nullptr;
		vk::Pipeline _pipeline = nullptr;

		vk::DescriptorSetLayout _setLayout;
		std::array<vk::DescriptorSet, RenderData::FRAMES_IN_FLIGHT> _sets;
	};
}
