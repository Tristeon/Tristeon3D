#pragma once
#include <set>
#include <string>
#include <vulkan/vulkan.hpp>
#include "Core/Serializable.h"
#include "Core/TypeRegister.h"

#ifdef TRISTEON_EDITOR
namespace Tristeon {
	namespace Editor {
		class ShaderFileItem;
	}
}
#endif

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			/**
			 * \brief ShaderFile is a class that is used to describe a shader's path and its properties
			 */
			class ShaderFile : public Serializable
			{
#ifdef TRISTEON_EDITOR
				friend Editor::ShaderFileItem;
#endif
			public:
				/**
				 * \brief Creates a new, empty shaderfile 
				 */
				ShaderFile() = default;
				/**
				 * \brief Creates a new shaderfile with a vertex and fragment shader
				 */
				ShaderFile(std::string vertexPath, std::string fragmentPath);

				void add(const std::string& path, const vk::ShaderStageFlagBits& stage);
				
				[[nodiscard]] bool ready() const { return _compiled && !_failed; }
				[[nodiscard]] bool failed() const { return _failed; }
				[[nodiscard]] std::map<vk::ShaderStageFlagBits, vk::ShaderModule> modules() const { return _modules; }
				[[nodiscard]] std::vector<vk::PipelineShaderStageCreateInfo> stages() const { return _stages; }

				/**
				* \brief Generates a json object containing the data of this shader file
				* \return Returns the json object
				*/
				nlohmann::json serialize() override;
				/**
				* \brief Deserializes the given json object into this object's data
				* \param json The json containing the serialized data
				*/
				void deserialize(nlohmann::json json) override;
			private:
				static std::vector<char> read(const std::string& path);
				static vk::ShaderModule create(const std::vector<char>& code);
				
				std::map<vk::ShaderStageFlagBits, vk::ShaderModule> _modules;
				std::map<vk::ShaderStageFlagBits, std::string> _paths;
				std::vector<vk::PipelineShaderStageCreateInfo> _stages;


				bool _compiled = false;
				bool _failed = false;
				
				REGISTER_TYPE_H(ShaderFile)
			};
		}
	}
}