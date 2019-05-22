#include "CollisionTesting.h"

REGISTER_TYPE_CPP(CollisionTestingTristan)

CollisionTestingTristan::CollisionTestingTristan()
{

}

nlohmann::json CollisionTestingTristan::serialize()
{
	nlohmann::json j = Component::serialize();
	j["typeID"] = TRISTEON_TYPENAME(CollisionTestingTristan);
	return j;
}

void CollisionTestingTristan::deserialize(nlohmann::json json)
{
}

void CollisionTestingTristan::start()
{
}

void CollisionTestingTristan::update()
{
}

void CollisionTestingTristan::onGUI()
{
}
