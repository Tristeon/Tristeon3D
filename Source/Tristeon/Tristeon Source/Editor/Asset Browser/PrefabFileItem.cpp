#include "PrefabFileItem.h"
#include "Editor/JsonSerializer.h"

void Tristeon::Editor::PrefabFileItem::CreatePrefab(Core::GameObject gameObject)
{
	JsonSerializer::serialize(filepath, gameObject);
}
