#include "Material.h"
#include "Editor/JsonSerializer.h"

namespace Tristeon 
{
	namespace Core 
	{
		namespace Rendering
		{
			REGISTER_TYPE_CPP(Material)

			nlohmann::json Material::serialize()
			{
				nlohmann::json j;
				j["typeID"] = typeid(Material).name();
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
				std::map<std::string, float> fl = json["floats"];

				//Checks if our serialized array contains the key [name], if so assignArr[name] = serArr[name], else assign a default value
#define validate(name, assignArr, serArr, defaultVal) \
	assignArr[name] = serArr.find(name) != serArr.end() ? serArr[name] : defaultVal; \
	break;
				//Update all properties
				for (auto pair : shader->getProps())
				{
					ShaderProperty p = pair.second;
					//Try to find the property in our material file
					//If we can't find it, assign standard values
					switch(p.valueType)
					{
						case DT_Image: validate(p.name, texturePaths, tex, "");
						case DT_Color: validate(p.name, colors, col, Misc::Color());
						case DT_Float: validate(p.name, floats, fl, 0);
						case DT_Vector3: validate(p.name, vectors, vec, Math::Vector3());
					}
				}
#undef validate
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
				if (std::experimental::filesystem::exists(shaderFilePath) &&
					std::experimental::filesystem::path(shaderFilePath).extension() == ".shader")
					shader = std::unique_ptr<ShaderFile>(JsonSerializer::deserialize<ShaderFile>(shaderFilePath));
				//Remove old shader
				else
					shader.reset();
			}
		}
	}
}
