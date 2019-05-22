#include "OBB.h"

using namespace Tristeon;
using namespace Tristan;

#include "Core/Transform.h"

REGISTER_TYPE_CPP(OBB)


void OBB::start()
{
	trans = transform.get();
}

bool OBB::collides(OBB obb)
{
	std::vector<Vector3> axises;
	return false;
}

std::vector<Vector3> OBB::getAxises()
{

	return std::vector<Vector3>();
}


nlohmann::json OBB::serialize()
{
	nlohmann::json j;
	j["typeID"] = TRISTEON_TYPENAME(OBB);
	return j;
}

void OBB::deserialize(nlohmann::json json)
{

}