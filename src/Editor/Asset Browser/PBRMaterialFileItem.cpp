#ifdef TRISTEON_EDITOR
#include "PBRMaterialFileItem.h"

#include <Core/JsonSerializer.h>
#include <Data/Resources.h>
#include <Core/Rendering/Materials/PBRMaterial.h>
#include <ImGUI/imgui.h>
#include <Editor/EditorImage.h>

namespace Tristeon::Editor
{
	REGISTER_TYPE_CPP(PBRMaterialFileItem);

	void PBRMaterialFileItem::init(std::string name, FolderItem* folder, std::string extension)
	{
		auto* material = Data::Resources::jsonLoad<Core::Rendering::PBRMaterial>(filepath);
		for (int i = 0; i < (int)Core::Rendering::PBRMaterial::TextureType::EndOfEnum; i++)
			editorImages[(Core::Rendering::PBRMaterial::TextureType)i] = EditorImage(material->texture((Core::Rendering::PBRMaterial::TextureType)i));
	}

	void PBRMaterialFileItem::drawOnInspector()
	{
		auto* material = Data::Resources::jsonLoad<Core::Rendering::PBRMaterial>(filepath);
		if (material == nullptr)
			return;

		for (int i = 0; i < (int)Core::Rendering::PBRMaterial::TextureType::EndOfEnum; i++)
		{
			if (ImGui::ImageButton(editorImages[(Core::Rendering::PBRMaterial::TextureType)i].getTextureID(), ImVec2{ 120, 120 }))
			{
				
			}
		}

		Core::JsonSerializer::serialize<Core::Rendering::PBRMaterial>(getFilePath(), *material);
	}

	nlohmann::json PBRMaterialFileItem::serialize()
	{
		nlohmann::json output = AssetItem::serialize();
		output["typeID"] = Tristeon::Core::Type<PBRMaterialFileItem>::fullName();
		return output;
	}
}

#endif