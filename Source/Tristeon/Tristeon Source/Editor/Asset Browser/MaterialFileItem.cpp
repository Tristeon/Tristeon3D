#include "MaterialFileItem.h"
#include "Editor/EditorImage.h"
#include "Editor/JsonSerializer.h"
#include "Core/Rendering/RenderManager.h"

namespace Tristeon
{
	namespace Editor
	{


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
			for (auto pair : shader->getProps())
				drawProperty(material, pair.second, "");

			JsonSerializer::serialize(getFilePath(), *material);
		}

		nlohmann::json MaterialFileItem::serialize()
		{
			nlohmann::json output = AssetItem::serialize();
			output["typeID"] = typeid(MaterialFileItem).name();
			return output;
		}

		void MaterialFileItem::drawProperty(Core::Rendering::Material* material, Core::Rendering::ShaderProperty p, std::string parent) const
		{
			std::string name = parent + p.name;
			switch (p.valueType)
			{
			case Core::Rendering::DT_Image:
			{
				std::string value = material->texturePaths[name];
				char charValue[255] = "";
				strcpy_s(charValue, 255, value.c_str());
				ImGui::InputText(name.c_str(), charValue, 255, ImGuiInputTextFlags_EnterReturnsTrue);
				material->setTexture(name, charValue);
				break;
			}
			case Core::Rendering::DT_Color:
			{
				auto value = material->colors[name].toArray();
				ImGui::InputFloat4(name.c_str(), value.data());
				material->setColor(name, Misc::Color(value[0], value[1], value[2], value[3]));
				break;
			}
			case Core::Rendering::DT_Float:
			{
				float value = material->floats[name];
				ImGui::InputFloat(name.c_str(), &value);
				material->setFloat(name, value);
				break;
			}
			case Core::Rendering::DT_Vector3:
			{
				auto value = material->vectors[name].toArray();
				ImGui::InputFloat3(name.c_str(), value.data());
				material->setVector3(name, Math::Vector3(value[0], value[1], value[2]));
				break;
			}
			case Core::Rendering::DT_Struct:
			{
				for (const auto c : p.children)
				{
					drawProperty(material, c, name + ".");
				}
				break;
			}
			}
		}

	}
}