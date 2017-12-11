#include "ShaderFile.h"
#include "Misc/Console.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			REGISTER_TYPE_CPP(ShaderFile)

			ShaderFile::ShaderFile()
			{
				//Empty
			}

			ShaderFile::ShaderFile(std::string name, std::string directory, std::string vertexName, std::string fragmentName, std::vector<ShaderProperty> properties) 
				: nameID(name), directory(directory), vertexName(vertexName), fragmentName(fragmentName), properties(properties)
			{
				//Empty
			}

			std::string ShaderFile::getPath(RenderAPI api, ShaderType type) const
			{
				//Get the name and the shader file extension of the api
				std::string apiName, apiExtension;
				switch (api)
				{
				case RAPI_Vulkan: 
					apiName = "Vulkan";
					apiExtension = ".spv";
					break;
				default:
					Misc::Console::error("Trying to create vertex shader with unsupported Graphics API!");
				}

				//Get the name of the file based on the shader type
				std::string fileName;
				switch(type)
				{
				case ST_Vertex:
					fileName = vertexName;
					break;
				case ST_Fragment: 
					fileName = fragmentName;
					break;
				default: 
					Misc::Console::error("Unsupported shader type!");
				}

				//Result
				return directory + fileName + apiName + apiExtension;
			}

			nlohmann::json ShaderFile::serialize()
			{
				nlohmann::json j;
				j["typeID"] = typeid(ShaderFile).name();
				j["nameID"] = nameID;
				j["directory"] = directory;
				j["vertexName"] = vertexName;
				j["fragmentName"] = fragmentName;
				j["properties"] = properties;
				return j;
			}

			void ShaderFile::deserialize(nlohmann::json json)
			{
				const std::string tempNameID = json["nameID"];
				nameID = tempNameID;

				const std::string tempDirectory = json["directory"];
				directory = tempDirectory;

				const std::string tempVertexName = json["vertexName"];
				vertexName = tempVertexName;

				const std::string tempFragmentName = json["fragmentName"];
				fragmentName = tempFragmentName;

				const std::vector<ShaderProperty> propertiesValue = json["properties"];
				properties = propertiesValue;
			}
		}
	}
}
