#include "MaterialFileItem.h"
#include "Editor/EditorImage.h"

void Tristeon::Editor::MaterialFileItem::drawOnInspector()
{
	Core::Rendering::Material* material = JsonSerializer::deserialize<Core::Rendering::Material>(getFilePath());
	Misc::Color diffuseColor = material->getDiffuse();
	float color[4]	{
		diffuseColor.r,
		diffuseColor.g,
		diffuseColor.b,
		diffuseColor.a
	};

	ImGui::ColorPicker4("Material",color);
}
