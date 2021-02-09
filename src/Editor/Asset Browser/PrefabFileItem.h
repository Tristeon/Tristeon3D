#pragma once
#ifdef TRISTEON_EDITOR

#include "AssetItem.h"

/**
 * \brief PrefabFileItem is an asset item that stores gameobject data
 */
class PrefabFileItem : public Tristeon::Editor::AssetItem
{
public:
	void SetPrefabData(nlohmann::json prefabData) const;
	nlohmann::json GetPrefabData() const;

	nlohmann::json serialize() override;
private:
	REGISTER_TYPE_H(PrefabFileItem)
};

#endif