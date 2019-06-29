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
	std::vector<Vector3> axises = getAxises();
	const auto iterator = axises.end();
	auto axises2 = obb.getAxises();
	axises.insert(iterator, axises2.begin(), axises2.end());
	return false;
}

std::vector<Vector3> OBB::getAxises()
{
	std::vector<Vector3> axes;
	for (int i = 0; i < 8; ++i)
	{

	}

	return std::vector<Vector3>();
}

Vector3 OBB::getPoints()
{
	std::vector<Vector3> points;
	auto point = size + trans->position;
	points.push_back(point); // Top right corner
	points.push_back(point * Vector3(-1, 1, 1));
	points.push_back(point * Vector3(-1, -1, 1));
	points.push_back(point * Vector3(-1, -1, -1));
	points.push_back(point * Vector3(1, -1, 1));
	points.push_back(point * Vector3(1, 1, -1));
	points.push_back(point * Vector3(1, -1, -1));
	points.push_back(point * Vector3(-1, 1, -1));

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