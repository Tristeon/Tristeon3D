#include "Core/Rendering/RenderManager.h"
#ifdef TRISTEON_EDITOR

#include "ShaderFileItem.h"
#include "Core/Rendering/ShaderFile.h"
#include "Editor/JsonSerializer.h"
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
	Core::Rendering::ShaderFile* file = JsonSerializer::deserialize<Core::Rendering::ShaderFile>(getFilePath());

	file->nameID = inputString("Name", file->nameID);
	file->directory = inputString("Shader directory", file->directory);
	file->vertexName = inputString("Vertex shader name", file->vertexName);
	file->fragmentName = inputString("Fragment shader name", file->fragmentName);

	if (ImGui::Button("Recompile"))
		Core::Rendering::RenderManager::recompileShader(getFilePath());

	JsonSerializer::serialize<Core::Rendering::ShaderFile>(getFilePath(), *file);
	delete file;
}

nlohmann::json ShaderFileItem::serialize()
{
	nlohmann::json output = AssetItem::serialize();
	output["typeID"] = Tristeon::Core::Type<ShaderFileItem>::fullName();
	return output;
}

#endif
