#ifdef TRISTEON_EDITOR
#include "Data/Resources.h"
#include "SkyboxFileItem.h"
#include "Core/Rendering/Skybox.h"
#include <ImGUI/imgui.h>
#include "Core/JsonSerializer.h"

namespace Tristeon::Editor
{
	REGISTER_TYPE_CPP(SkyboxFileItem)

	void drawTexName(std::string label, std::string& value)
	{
		char c[255] = "";
		strcpy_s(c, 255, value.c_str());
		ImGui::InputText(label.c_str(), c, 255, ImGuiInputTextFlags_EnterReturnsTrue);
		value = c;
	}

	void SkyboxFileItem::drawOnInspector()
	{
		auto* skybox = Data::Resources::jsonLoad<Core::Rendering::Skybox>(filepath);
		if (!skybox)
			return;

		drawTexName("Texture", skybox->texturePath);
		Core::JsonSerializer::serialize(getFilePath(), *skybox);
	}

	nlohmann::json SkyboxFileItem::serialize()
	{
		nlohmann::json j = AssetItem::serialize();
		j["typeID"] = Tristeon::Core::Type<SkyboxFileItem>::fullName();
		return j;
	}
}

#endif