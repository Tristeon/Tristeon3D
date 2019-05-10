#pragma once
#include <Core/Components/Component.h>
#include "Math/Vector3.h"
#include "Misc/Color.h"

struct OBB
{
	Tristeon::Vector3 position;
	Tristeon::Vector3 rotation;
	float size = 0;
};

class CollisionTestingLeon : public Tristeon::Core::Components::Component
{
	REGISTER_TYPE_H(CollisionTestingLeon)
public:
	CollisionTestingLeon() = default;
	~CollisionTestingLeon() = default;
	nlohmann::json serialize() override;
	void deserialize(nlohmann::json json) override;
	void start() override;
	void update() override;
	void onGUI() override;
private:
	bool checkOBBonOBB();
	OBB colliderA;
	OBB colliderB;

	int movingID;
};
