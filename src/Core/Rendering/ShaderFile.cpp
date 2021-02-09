#include "ShaderFile.h"
#include <fstream>

#include "Core/BindingData.h"

namespace Tristeon::Core::Rendering
{
	REGISTER_TYPE_CPP(ShaderFile)

	ShaderFile::ShaderFile(std::string vertexPath, std::string fragmentPath)
	{
		add(vertexPath, vk::ShaderStageFlagBits::eVertex);
		add(fragmentPath, vk::ShaderStageFlagBits::eFragment);
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

	nlohmann::json ShaderFile::serialize()
	{
		nlohmann::json j;
		j["typeID"] = Type<ShaderFile>::fullName();

		nlohmann::json::array_t arr;
		for (auto [stage, path] : _paths)
		{
			nlohmann::json shader;
			shader["stage"] = (uint32_t)stage;
			shader["path"] = path;
			arr.push_back(shader);
		}
		
		j["shaders"] = arr;
		return j;
	}

	void ShaderFile::deserialize(nlohmann::json json)
	{
		nlohmann::json::array_t arr = json.value("shaders", nlohmann::json::array_t());
		for (auto element : arr)
		{
			_paths[(vk::ShaderStageFlagBits)element.value("stage", uint32_t())] = element.value("path", std::string());
		}
		
		for (auto [stage, path] : _paths)
			add(path, stage);
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
		return binding_data.device.createShaderModule(module_ci);
	}
}