#ifdef TRISTEON_EDITOR

#include "SkyboxFileItem.h"
#include "Core/Rendering/Skybox.h"
#include "Core/Rendering/RenderManager.h"
#include <ImGUI/imgui.h>
#include "Editor/JsonSerializer.h"
#include "XPlatform/typename.h"

namespace Tristeon
{
	namespace Editor
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
			Core::Rendering::Skybox* skybox = Core::Rendering::RenderManager::getSkybox(getFilePath());
			if (!skybox)
				return;

			drawTexName("Texture", skybox->texturePath);
			JsonSerializer::serialize<Core::Rendering::Skybox>(getFilePath(), *skybox);
		}

		nlohmann::json SkyboxFileItem::serialize()
		{
			nlohmann::json j = AssetItem::serialize();
			j["typeID"] = TRISTEON_TYPENAME(SkyboxFileItem);
			return j;
		}
	}
}

#endif
