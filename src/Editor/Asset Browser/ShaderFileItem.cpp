#ifdef TRISTEON_EDITOR
#include "ShaderFileItem.h"
#include "Core/Rendering/ShaderFile.h"
#include "Core/JsonSerializer.h"
#include <ImGUI/imgui.h>

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
	Core::Rendering::ShaderFile* file = Core::JsonSerializer::deserialize<Core::Rendering::ShaderFile>(getFilePath());

	file->directory = inputString("Shader directory", file->directory);
	file->vertexName = inputString("Vertex shader name", file->vertexName);
	file->fragmentName = inputString("Fragment shader name", file->fragmentName);

	//TODO: Re-enable shader recompile
	//if (ImGui::Button("Recompile"))
		//file->recompile();

	Core::JsonSerializer::serialize<Core::Rendering::ShaderFile>(getFilePath(), *file);
	delete file;
}

nlohmann::json ShaderFileItem::serialize()
{
	nlohmann::json output = AssetItem::serialize();
	output["typeID"] = Tristeon::Core::Type<ShaderFileItem>::fullName();
	return output;
}

#endif
