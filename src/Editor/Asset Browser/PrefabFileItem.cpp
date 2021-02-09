#ifdef TRISTEON_EDITOR

#include "PrefabFileItem.h"
#include "Core/JsonSerializer.h"

REGISTER_TYPE_CPP(PrefabFileItem)

void PrefabFileItem::SetPrefabData(nlohmann::json prefabData) const
{
	Tristeon::Core::JsonSerializer::serialize(getFilePath(), prefabData);
}

nlohmann::json PrefabFileItem::GetPrefabData() const
{
	return Tristeon::Core::JsonSerializer::load(getFilePath());
}

nlohmann::json PrefabFileItem::serialize()
{
	nlohmann::json output = AssetItem::serialize();
	output["typeID"] = Tristeon::Core::Type<PrefabFileItem>::fullName();
	return output;
}

#endif
