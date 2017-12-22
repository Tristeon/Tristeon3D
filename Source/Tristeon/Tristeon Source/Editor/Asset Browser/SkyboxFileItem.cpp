#include "SkyboxFileItem.h"
#include "Core/Rendering/Skybox.h"
#include "Core/Rendering/RenderManager.h"
#include "../../../ImGUI/imgui.h"
#include "Editor/JsonSerializer.h"

namespace Tristeon
{
	namespace Editor
	{
		REGISTER_TYPE_CPP(SkyboxFileItem)

		void drawTexName(std::string label, std::string& value, bool& isDirty)
		{
			char c[255] = "";
			strcpy_s(c, 255, value.c_str());
			ImGui::InputText(label.c_str(), c, 255, ImGuiInputTextFlags_EnterReturnsTrue);
			if (value != c)
				isDirty = true;
			value = c;
		}

		void SkyboxFileItem::drawOnInspector()
		{
			Core::Rendering::Skybox* skybox = Core::Rendering::RenderManager::getSkybox(getFilePath());
			if (!skybox)
				return;

			bool isDirty = false;
			drawTexName("Left", skybox->texNames.left, isDirty);
			drawTexName("Right", skybox->texNames.right, isDirty);
			drawTexName("Up", skybox->texNames.up, isDirty);
			drawTexName("Down", skybox->texNames.down, isDirty);
			drawTexName("Forward", skybox->texNames.forward, isDirty);
			drawTexName("Backward", skybox->texNames.back, isDirty);

			if (isDirty)
				skybox->updateTextures();

			JsonSerializer::serialize<Core::Rendering::Skybox>(getFilePath(), *skybox);
		}

		nlohmann::json SkyboxFileItem::serialize()
		{
			nlohmann::json j = AssetItem::serialize();
			j["typeID"] = typeid(SkyboxFileItem).name();
			return j;
		}
	}
}