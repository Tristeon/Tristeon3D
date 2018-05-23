#include "ShaderFile.h"
#include "Misc/Console.h"
#include <spirv_cross/spirv_cross.hpp>
#include "Core/Settings.h"
#include <fstream>
#include "XPlatform/typename.h"

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

			ShaderFile::ShaderFile(std::string name, std::string directory, std::string vertexName, std::string fragmentName)
				: nameID(name), directory(directory), vertexName(vertexName), fragmentName(fragmentName)
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
				switch (type)
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
				j["typeID"] = TRISTEON_TYPENAME(ShaderFile);
				j["nameID"] = nameID;
				j["directory"] = directory;
				j["vertexName"] = vertexName;
				j["fragmentName"] = fragmentName;
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
			}

			std::map<int, ShaderProperty> ShaderFile::getProps()
			{
				if (loadedProps)
					return properties;

				properties.clear();

				RenderAPI const rapi = Settings::getRenderAPI();

				//Vertex
				std::ifstream in_vert(getPath(rapi, ST_Vertex), std::ifstream::binary);
				std::ifstream in_frag(getPath(rapi, ST_Fragment), std::ifstream::binary);

				if (!in_vert || !in_vert.good() || !in_frag || !in_frag.good())
				{
					Misc::Console::error("Failed to open shader files!");
					return {};
				}

				in_vert.seekg(0, in_vert.end);
				auto const vertN = in_vert.tellg();
				in_vert.seekg(0, in_vert.beg);

				std::vector<uint32_t> vert_buf(vertN / sizeof(uint32_t));// reserve space for N/8 doubles
				in_vert.read(reinterpret_cast<char*>(vert_buf.data()), vert_buf.size() * sizeof(uint32_t));

				//Fragment
				in_frag.seekg(0, in_frag.end);
				auto const fragN = in_frag.tellg();
				in_frag.seekg(0, in_frag.beg);

				std::vector<uint32_t> frag_buf(fragN / sizeof(uint32_t));// reserve space for N/8 doubles
				in_frag.read(reinterpret_cast<char*>(frag_buf.data()), frag_buf.size() * sizeof(uint32_t));

				//Compilers
				spirv_cross::Compiler vertexComp = spirv_cross::Compiler(vert_buf);
				spirv_cross::Compiler fragmentComp = spirv_cross::Compiler(frag_buf);

				applyShaderReflection(vertexComp, Vertex);
				applyShaderReflection(fragmentComp, Fragment);

				loadedProps = true;
				return properties;
			}

			bool ShaderFile::hasVariable(int set, int binding, DataType data, ShaderType stage)
			{
				spirv_cross::Compiler compiler = getCompiler(Settings::getRenderAPI(), stage);

				spirv_cross::ShaderResources res = compiler.get_shader_resources();

				switch(data)
				{
				case DT_Image:
					for (const auto s : res.sampled_images)
					{
						if (compiler.get_decoration(s.id, spv::DecorationDescriptorSet) != set ||
							compiler.get_decoration(s.id, spv::DecorationBinding) != binding)
							continue;
						return true;
					}
					break;
				case DT_Color:
				case DT_Float:
				case DT_Vector3:
				case DT_Struct:
					for (const auto u : res.uniform_buffers)
					{
						if (compiler.get_decoration(u.id, spv::DecorationDescriptorSet) != set ||
							compiler.get_decoration(u.id, spv::DecorationBinding) != binding)
							continue;
						return true;
					}
					break;
				}


				return false;
			}

			spirv_cross::Compiler ShaderFile::getCompiler(RenderAPI rapi, ShaderType stage) const
			{
				std::ifstream in(getPath(rapi, stage), std::ifstream::binary);
				in.seekg(0, in.end);
				auto const n = in.tellg();
				in.seekg(0, in.beg);

				std::vector<uint32_t> buf(n / sizeof(uint32_t));// reserve space for N/8 doubles
				in.read(reinterpret_cast<char*>(buf.data()), buf.size() * sizeof(uint32_t));

				return spirv_cross::Compiler(buf);
			}

			ShaderProperty getProperty(spirv_cross::Compiler& comp, std::string name, uint32_t typeID, uint32_t variableID, ShaderStage stage)
			{
				ShaderProperty prop;
				prop.name = name;
				prop.shaderStage = stage;
				prop.valueType = DT_Unknown; //Initial value
				prop.size = 0;

				spirv_cross::SPIRType t = comp.get_type(typeID);

				if (t.vecsize == 3)
				{
					prop.valueType = DT_Vector3;
					prop.size = t.vecsize * sizeof(float);
				}
				if (t.vecsize == 4)
				{
					prop.valueType = DT_Color;
					prop.size = t.vecsize * sizeof(float);
				}
				else
				{
					switch (t.basetype)
					{
					case spirv_cross::SPIRType::Unknown:
						Misc::Console::warning("Shader uniform type unknown not supported!");
						break;
					case spirv_cross::SPIRType::Void:
						Misc::Console::warning("Shader uniform type void not supported!");
						break;
					case spirv_cross::SPIRType::Boolean:
						Misc::Console::warning("Shader uniform type boolean not supported! TODO");
						break;
					case spirv_cross::SPIRType::Char:
						Misc::Console::warning("Shader uniform type char not supported! TODO");
						break;
					case spirv_cross::SPIRType::Int:
						Misc::Console::warning("Shader uniform type int not supported! TODO");
						break;
					case spirv_cross::SPIRType::UInt:
						Misc::Console::warning("Shader uniform type unsigned int not supported! TODO");
						break;
					case spirv_cross::SPIRType::Int64:
						Misc::Console::warning("Shader uniform type int 64 not supported! TODO");
						break;
					case spirv_cross::SPIRType::UInt64:
						Misc::Console::warning("Shader uniform type unsigned int 64 not supported! TODO");
						break;
					case spirv_cross::SPIRType::Float:
					{
						prop.valueType = DT_Float;
						prop.size = sizeof(float);
						break;
					}
					case spirv_cross::SPIRType::AtomicCounter:
						Misc::Console::warning("Shader uniform type atomic counter not supported!");
						break;
					case spirv_cross::SPIRType::Double:
						Misc::Console::warning("Shader uniform type double not supported! TODO");
						break;
					case spirv_cross::SPIRType::Struct:
					{
						if (t.member_types.size() > 0)
						{
							for (int i = 0; i < t.member_types.size(); i++)
							{
								spirv_cross::SPIRType memType = comp.get_type(t.member_types[i]);
								std::string const n = comp.get_member_name(typeID, i);
								ShaderProperty const p = getProperty(comp, n, t.member_types[i], variableID, stage);
								prop.children.push_back(p);
							}

							prop.valueType = DT_Struct;
							for (const auto c : prop.children)
								prop.size += c.size;
						}
						break;
					}
					}
				}
				return prop;
			}

			void ShaderFile::applyShaderReflection(spirv_cross::Compiler& comp, ShaderStage stage)
			{
				spirv_cross::ShaderResources res = comp.get_shader_resources();

				for (const auto u : res.uniform_buffers)
				{
					unsigned int const set = comp.get_decoration(u.id, spv::DecorationDescriptorSet);
					if (set != 1)
						continue;

					if (u.name != "UniformBufferObject")
					{
						unsigned int const binding = comp.get_decoration(u.id, spv::DecorationBinding);

						ShaderProperty const p = getProperty(comp, u.name, u.base_type_id, u.id, stage);
						if (p.valueType != DT_Unknown)
							properties[binding] = p;
					}
				}

				for (const auto s : res.sampled_images)
				{
					unsigned int const set = comp.get_decoration(s.id, spv::DecorationDescriptorSet);
					if (set != 1)
						continue;

					ShaderProperty prop;
					prop.name = s.name;
					prop.shaderStage = stage;
					prop.valueType = DT_Image;
					prop.size = 0;

					unsigned int const binding = comp.get_decoration(s.id, spv::DecorationBinding);
					properties[binding] = prop;
				}
				//TODO: Support shader storage buffers
				//TODO: Support shader constant buffers
			}
		}
	}
}
