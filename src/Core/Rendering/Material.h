#pragma once
#include "Core/TObject.h"
#include "ShaderFile.h"

#include <vulkan/vulkan.hpp>
#include <glm/mat4x4.hpp>

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
		REGISTER_TYPE_H(Material);
		
		friend class RenderManager;
#ifdef TRISTEON_EDITOR
		friend Editor::MaterialFileItem;
#endif
	public:
		Material();
		explicit Material(PipelineProperties properties);
		~Material();

		[[nodiscard]] vk::Pipeline pipeline()
		{
			if (!_pipeline)
				buildPipeline();
			return _pipeline;
		}
		virtual void buildPipeline();
		virtual ShaderFile* shader();
	protected:
		/**
		 * \brief Prepare function, override this function to send data to the gpu
		 * \param model The transformation matrix of the object
		 * \param view The current camera view matrix
		 * \param proj The current camera projection matrix
		 */
		virtual void prepare(glm::mat4 model, glm::mat4 view, glm::mat4 proj) { }

		/**
		 * \brief The filepath to the shader file
		 */
		std::string shaderFilePath;

		PipelineProperties _properties{};
		vk::PipelineLayout _layout = nullptr;
		vk::Pipeline _pipeline = nullptr;
	};
}