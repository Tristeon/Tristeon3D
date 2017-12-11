#include "MaterialFileItem.h"
#include "Editor/EditorImage.h"
#include "Editor/JsonSerializer.h"
#include "Core/Rendering/RenderManager.h"

using namespace Tristeon::Editor;

REGISTER_TYPE_CPP(MaterialFileItem)

void MaterialFileItem::drawOnInspector()
{
	Core::Rendering::Material* material = Core::Rendering::RenderManager::getMaterial(getFilePath());
	if (material == nullptr)
		return;

	//Shader file
	char c[255] = "";
	strcpy_s(c, 255, material->shaderFilePath.c_str());
	ImGui::InputText("Shader File Path", c, 255, ImGuiInputTextFlags_EnterReturnsTrue);
	material->shaderFilePath = c;
	material->updateShader();

	if (material->shader == nullptr)
		return;

	Core::Rendering::ShaderFile* shader = material->shader.get();
	for (auto p : shader->getProperties())
	{
		switch(p.valueType)
		{
			case Core::Rendering::DT_Image:
			{
				std::string value = material->texturePaths[p.name];
				char charValue[255] = "";
				strcpy_s(charValue, 255, value.c_str());
				ImGui::InputText(p.name.c_str(), charValue, 255, ImGuiInputTextFlags_EnterReturnsTrue);
				material->setTexture(p.name, charValue);
				break;
			}
			case Core::Rendering::DT_Color:
			{
				auto value = material->colors[p.name].toArray();
				ImGui::InputFloat4(p.name.c_str(), value.data());
				material->setColor(p.name, Misc::Color(value[0], value[1], value[2], value[3]));
				break;
			}
			case Core::Rendering::DT_Float:
			{
				float value = material->floats[p.name];
				ImGui::InputFloat(p.name.c_str(), &value);
				material->setFloat(p.name, value);
				break;
			}
			case Core::Rendering::DT_Vector3:
			{
				auto value = material->vectors[p.name].toArray();
				ImGui::InputFloat3(p.name.c_str(), value.data());
				material->setVector3(p.name, Math::Vector3(value[0], value[1], value[2]));
				break;
			}
		}
	}
	
	JsonSerializer::serialize(getFilePath(), *material);
}

nlohmann::json MaterialFileItem::serialize()
{
	nlohmann::json output = AssetItem::serialize();
	output["typeID"] = typeid(MaterialFileItem).name();
	return output;
}	