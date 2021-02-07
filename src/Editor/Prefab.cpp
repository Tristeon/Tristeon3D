﻿#ifdef TRISTEON_EDITOR

#include "Prefab.h"

nlohmann::json Tristeon::Prefab::getPrefabData()
{
	return prefabFile->GetPrefabData();
}

nlohmann::json Tristeon::Prefab::serialize()
{
	nlohmann::json output;
	output["typeID"] = Core::Type<Prefab>::fullName();
	output["prefabFilePath"] = prefabFilePath;
	return output;
}

void Tristeon::Prefab::deserialize(nlohmann::json json)
{
	const std::string prefabFilePathData = json["prefabFilePath"];
	prefabFilePath = prefabFilePathData;
}

#endif
