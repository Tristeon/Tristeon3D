#pragma once
#include "Core/TObject.h"
#include "ShaderFile.h"

#include <vulkan/vulkan.hpp>
#include <Core/TypeRegister.h>

#ifdef TRISTEON_EDITOR
namespace Tristeon {
	namespace Editor {
		class MaterialFileItem;
	}
}
#endif

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
#ifdef TRISTEON_EDITOR
		friend Editor::MaterialFileItem;
#endif
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

		[[nodiscard]] vk::DescriptorSet set()
		{
			if (!_set)
				createDescriptorSets();
			return _set;
		}

		virtual void createPipeline() = 0;
		virtual void createDescriptorSets() = 0;
		virtual ShaderFile* shader() = 0;
	protected:
		PipelineProperties _properties{};
		vk::PipelineLayout _layout = nullptr;
		vk::Pipeline _pipeline = nullptr;
		vk::DescriptorSet _set = nullptr;
		vk::DescriptorSetLayout _setLayout;
	};
}