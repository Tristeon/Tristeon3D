#pragma once
#include <string>
#include <map>

#include <vulkan/vulkan.hpp>

namespace Tristeon::Core::Rendering
{
	/**
	 * \brief ShaderFile is a class that is used to describe a shader's path and its properties
	 */
	class ShaderFile
	{
		friend class RenderManager;
	public:
		/**
		 * \brief Creates a new, empty shaderfile
		 */
		ShaderFile();
		/**
		 * \brief Creates a new shaderfile with a vertex and fragment shader
		 */
		ShaderFile(std::string vertexPath, std::string fragmentPath);

		~ShaderFile();

		void add(const std::string& path, const vk::ShaderStageFlagBits& stage);

		[[nodiscard]] bool ready() const { return _compiled && !_failed; }
		[[nodiscard]] bool failed() const { return _failed; }
		[[nodiscard]] std::map<vk::ShaderStageFlagBits, vk::ShaderModule> modules() const { return _modules; }
		[[nodiscard]] std::vector<vk::PipelineShaderStageCreateInfo> stages() const { return _stages; }

	private:
		static std::vector<char> read(const std::string& path);
		static vk::ShaderModule create(const std::vector<char>& code);

		void clean();

		std::map<vk::ShaderStageFlagBits, vk::ShaderModule> _modules;
		std::map<vk::ShaderStageFlagBits, std::string> _paths;
		std::vector<vk::PipelineShaderStageCreateInfo> _stages;

		bool _compiled = false;
		bool _failed = false;
	};
}
