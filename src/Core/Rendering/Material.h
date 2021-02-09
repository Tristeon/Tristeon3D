#pragma once
#include "Core/TObject.h"
#include "ShaderFile.h"

#include <vulkan/vulkan.hpp>
#include <glm/mat4x4.hpp>

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
		explicit Material(ShaderFile shader, PipelineProperties properties);
		~Material();

		/**
		 * \brief Serializes the material into a json file
		 * \return Returns the json object describing the material's information
		 */
		nlohmann::json serialize() override;
		/**
		 * \brief Deserializes the material from a json file
		 * \param json Returns the json object describing the material's information
		 */
		void deserialize(nlohmann::json json) override;

		[[nodiscard]] vk::Pipeline pipeline() const { return _pipeline; }
		virtual void buildPipeline();
	protected:
		/**
		 * \brief Prepare function, override this function to send data to the shader
		 * \param model The transformation matrix of the object
		 * \param view The current camera view matrix
		 * \param proj The current camera projection matrix
		 */
		virtual void prepare(glm::mat4 model, glm::mat4 view, glm::mat4 proj) { }
		
		virtual void updateShader();

		/**
		* \brief The shaderfile, loaded using shaderFilePath
		*/
		std::unique_ptr<ShaderFile> shader;
	private:
		/**
		 * \brief The filepath to the shader file
		 */
		std::string shaderFilePath;

		PipelineProperties _properties;
		vk::PipelineLayout _layout;
		vk::Pipeline _pipeline;
	};
}