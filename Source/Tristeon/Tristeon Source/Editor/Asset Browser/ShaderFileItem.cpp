#include "ShaderFileItem.h"
#include "Core/Rendering/ShaderFile.h"
#include "Editor/JsonSerializer.h"
#include "../../../ImGUI/imgui.h"

using namespace Tristeon::Editor;

REGISTER_TYPE_CPP(ShaderFileItem)

std::string inputString(std::string label, std::string input)
{
	//Shader file
	char c[255] = "";
	strcpy_s(c, 255, input.c_str());
	ImGui::InputText(label.c_str(), c, 255, ImGuiInputTextFlags_EnterReturnsTrue);
	return c;
}

void ShaderFileItem::drawOnInspector()
{
	Core::Rendering::ShaderFile* file = JsonSerializer::deserialize<Core::Rendering::ShaderFile>(getFilePath());

	file->nameID = inputString("Name", file->nameID);
	file->directory = inputString("Shader directory", file->directory);
	file->vertexName = inputString("Vertex shader name", file->vertexName);
	file->fragmentName = inputString("Fragment shader name", file->fragmentName);

	if (ImGui::Button("Add property"))
		file->properties.push_back(Core::Rendering::ShaderProperty());

	if (ImGui::CollapsingHeader("Properties"))
	{
		auto removeIt = file->properties.end();
		for (auto i = file->properties.begin(); i < file->properties.end(); ++i)
		{
			Core::Rendering::ShaderProperty p = *i;
			//TODO: Shader file item box and fix multiple items
			ImGui::Separator();
			ImGui::Text(p.name.c_str());
			ImGui::SameLine();
			if(ImGui::Button("Remove"))
			{
				removeIt = i;
				continue;
			}

			p.name = inputString("Property name", p.name);

			std::array<char*, 4> options = { "Image", "Color", "Float", "Vector3" };
			int value = (int)p.valueType;
			ImGui::Combo("Type", &value, options.data(), 4);
			p.valueType = (Core::Rendering::DataType)value;

			std::array<char*, 6> options1 = { "Vertex", "Fragment", "Geometry", "Compute", "All Graphics", "All" };
			int value2 = (int)p.shaderStage;
			ImGui::Combo("Shader stage", &value2, options1.data(), 6);
			p.shaderStage = (Core::Rendering::ShaderStage)value2;

			*i = p;
		}

		if (removeIt != file->properties.end())
			file->properties.erase(removeIt);
	}

	JsonSerializer::serialize<Core::Rendering::ShaderFile>(getFilePath(), *file);
	delete file;
}

nlohmann::json ShaderFileItem::serialize()
{
	nlohmann::json output = AssetItem::serialize();
	output["typeID"] = typeid(ShaderFileItem).name();
	return output;
}