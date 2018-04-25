#if TRISTEON_EDITOR

#include "PrefabFileItem.h"
#include "Editor/JsonSerializer.h"

DerivedRegister<PrefabFileItem> PrefabFileItem::reg;

void PrefabFileItem::SetPrefabData(nlohmann::json prefabData) const
{
	JsonSerializer::serialize(getFilePath(), prefabData);
}

nlohmann::json PrefabFileItem::GetPrefabData() const
{
	return JsonSerializer::load(getFilePath());
}

nlohmann::json PrefabFileItem::serialize()
{
	nlohmann::json output = AssetItem::serialize();
	output["typeID"] = typeid(PrefabFileItem).name();
	return output;
}

#endif