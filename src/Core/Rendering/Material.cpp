#include "Material.h"
#include "Editor/JsonSerializer.h"
#include <filesystem>

namespace Tristeon::Core::Rendering
{
	REGISTER_TYPE_CPP(Material)

	nlohmann::json Material::serialize()
	{
		nlohmann::json j;
		j["typeID"] = Type<Material>::fullName();
		j["texturePaths"] = texturePaths;
		j["vectors"] = vectors;
		j["colors"] = colors;
		j["floats"] = floats;
		j["shaderFilePath"] = shaderFilePath;
		return j;
	}

	void Material::deserialize(nlohmann::json json)
	{
		//Get the shader file
		const std::string shaderFilePathValue = json["shaderFilePath"];
		//Only update our shader if our path has changed
		if (shaderFilePath != shaderFilePathValue)
		{
			shaderFilePath = shaderFilePathValue;
			updateShader();
			//Update our info
			updateProperties(false);
		}

		//If we don't have a shader there's also no material property data to deserialize
		//(or at least we'll just assume so)
		if (shader == nullptr)
			return;

		//Get texture paths map from json
		const nlohmann::json& jtex = json["texturePaths"];
		std::map<std::string, std::string> tex;
		for (auto& element : nlohmann::json::iterator_wrapper(jtex))
		{
			const std::string val = element.value();
			tex[element.key()] = val;
		}

		//Get vectors from json
		const nlohmann::json& jvec = json["vectors"];
		std::map<std::string, Math::Vector3> vec;
		for (auto& element : nlohmann::json::iterator_wrapper(jvec))
			vec[element.key()] = element.value();

		//Get colors from json
		const nlohmann::json& jcol = json["colors"];
		std::map<std::string, Misc::Color> col;
		for (auto& element : nlohmann::json::iterator_wrapper(jcol))
			col[element.key()] = element.value();

		//Get floats from json
		std::map<std::string, float> const fl = json["floats"];

		//Validate properties
		for (auto const pair : shader->getProps())
		{
			ShaderProperty const p = pair.second;
			checkProperty("", p, tex, vec, col, fl);
		}
	}

	void Material::setTexture(std::string name, std::string path)
	{
		//Validate if the property exists
		if (texturePaths.find(name) != texturePaths.end())
			texturePaths[name] = path;
		else
			Misc::Console::warning("Trying to set material texture [" + name + "] but that variable is not defined in shader.properties!");
	}

	void Material::setFloat(std::string name, float value)
	{
		//Validate if the property exists
		if (floats.find(name) != floats.end())
			floats[name] = value;
		else
			Misc::Console::warning("Trying to set material float [" + name + "] but that variable is not defined in shader.properties!");
	}

	void Material::setVector3(std::string name, Math::Vector3 value)
	{
		//Validate if the property exists
		if (vectors.find(name) != vectors.end())
			vectors[name] = value;
		else
			Misc::Console::warning("Trying to set material vector [" + name + "] but that variable is not defined in shader.properties!");
	}

	void Material::setColor(std::string name, Misc::Color value)
	{
		//Validate if the property exists
		if (colors.find(name) != colors.end())
			colors[name] = value;
		else
			Misc::Console::warning("Trying to set material color [" + name + "] but that variable is not defined in shader.properties!");
	}

	void Material::updateShader()
	{
		//Try to set it if possible
		if (std::filesystem::exists(shaderFilePath) && std::filesystem::path(shaderFilePath).extension() == ".shader")
			shader = std::unique_ptr<ShaderFile>(JsonSerializer::deserialize<ShaderFile>(shaderFilePath));
		//Remove old shader
		else
			shader.reset();
	}

	void Material::checkProperty(std::string parentName, ShaderProperty p, const std::map<std::string, std::string>& tex,
		const std::map<std::string, Math::Vector3>& vec, const std::map<std::string, Misc::Color>& col,
		const std::map<std::string, float>& fl)
	{
		//Try to get data out of the given vectors,
		//If we can't find it assign standard variable
		std::string const name = parentName + p.name;
		switch (p.valueType)
		{
		case DT_Image: texturePaths[name] = tex.find(name) != tex.end() ? tex.at(name) : ""; break;
		case DT_Color: colors[name] = col.find(name) != col.end() ? col.at(name) : Misc::Color(); break;
		case DT_Float: floats[name] = fl.find(name) != fl.end() ? fl.at(name) : 0; break;
		case DT_Vector3: vectors[name] = vec.find(name) != vec.end() ? vec.at(name) : Math::Vector3(); break;
		case DT_Struct:
		{
			for (const auto c : p.children)
				checkProperty(name + ".", c, tex, vec, col, fl);
			break;
		}
		}
	}
}
