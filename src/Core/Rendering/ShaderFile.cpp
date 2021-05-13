#include "ShaderFile.h"
#include <fstream>

#include "Core/RenderData.h"
#include "Core/Collector.h"

namespace Tristeon::Core::Rendering
{
	ShaderFile::ShaderFile()
	{
		Collector<ShaderFile>::add(this);
	}

	ShaderFile::ShaderFile(std::string vertexPath, std::string fragmentPath)
	{
		Collector<ShaderFile>::add(this);
		
		add(vertexPath, vk::ShaderStageFlagBits::eVertex);
		add(fragmentPath, vk::ShaderStageFlagBits::eFragment);
	}

	ShaderFile::~ShaderFile()
	{
		Collector<ShaderFile>::remove(this);
	}

	void ShaderFile::add(const std::string& path, const vk::ShaderStageFlagBits& stage)
	{
		if (_modules.find(stage) != _modules.end())
			return;

		const auto data = read(path);
		const auto mod = create(data);
		_modules[stage] = mod;

		_stages.push_back(vk::PipelineShaderStageCreateInfo{ {}, stage, mod, "main", nullptr });
	}

	std::vector<char> ShaderFile::read(const std::string& path)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}
		
		const auto size = file.tellg();
		std::vector<char> result(size);

		file.seekg(0);
		file.read(result.data(), size);
		file.close();

		return result;
	}

	vk::ShaderModule ShaderFile::create(const std::vector<char>& code)
	{
		const vk::ShaderModuleCreateInfo module_ci{ {}, code.size(), reinterpret_cast<const uint32_t*>(code.data()) };
		return renderData.device.createShaderModule(module_ci);
	}

	void ShaderFile::clean()
	{
		_compiled = false;
		_failed = false;

		for (auto [stage, m] : _modules)
			renderData.device.destroyShaderModule(m);
	}
}
