#include "CollisionTesting.h"
#include "OBB.h"

using namespace Tristan;
using namespace Tristeon;


REGISTER_TYPE_CPP(CollisionTesting)

CollisionTesting::CollisionTesting()
{

}

nlohmann::json CollisionTesting::serialize()
{
	nlohmann::json j = Component::serialize();
	j["typeID"] = TRISTEON_TYPENAME(CollisionTesting);
	return j;
}

void CollisionTesting::deserialize(nlohmann::json json)
{
}

void CollisionTesting::start()
{
}

void CollisionTesting::update()
{
}