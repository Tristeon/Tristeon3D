#ifdef TRISTEON_EDITOR

#include "PrefabFileItem.h"
#include "Editor/JsonSerializer.h"
#include "XPlatform/typename.h"

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
	output["typeID"] = TRISTEON_TYPENAME(PrefabFileItem);
	return output;
}

#endif
