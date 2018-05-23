#ifdef TRISTEON_EDITOR

#include "Prefab.h"
#include "XPlatform/typename.h"

nlohmann::json Tristeon::Prefab::getPrefabData()
{
	return prefabFile->GetPrefabData();
}

nlohmann::json Tristeon::Prefab::serialize()
{
	nlohmann::json output;
	output["typeID"] = TRISTEON_TYPENAME(Prefab);
	output["prefabFilePath"] = prefabFilePath;
	return output;
}

void Tristeon::Prefab::deserialize(nlohmann::json json)
{
	const std::string prefabFilePathData = json["prefabFilePath"];
	prefabFilePath = prefabFilePathData;
}

#endif
