#include "OBB.h"
#include "Projection.h"

using namespace Tristeon;
using namespace Tristan;

#include "Core/Transform.h"

REGISTER_TYPE_CPP(OBB)


void OBB::start()
{
	trans = transform.get();
}

bool OBB::collides(OBB obb) const
{
	vector<Vector3> allAxises = getAllAxises(obb);

	for (Vector3 axis : allAxises)
	{
		Projection p1 = project(axis);
		Projection p2 = obb.project(axis);

		if (!p1.overlap(p2))
			return false;
	}
	return true;
}

vector<Vector3> OBB::getAxises() const
{
	return { trans->right(), trans->up(), trans->forward() };
}

vector<Vector3> OBB::getPoints() const
{
	vector<Vector3> points;
	const auto point = size + trans->position;
	points.push_back(point); // Top right corner
	points.push_back(point * Vector3(-1, 1, 1));
	points.push_back(point * Vector3(-1, -1, 1));
	points.push_back(point * Vector3(-1, -1, -1));
	points.push_back(point * Vector3(1, -1, 1));
	points.push_back(point * Vector3(1, 1, -1));
	points.push_back(point * Vector3(1, -1, -1));
	points.push_back(point * Vector3(-1, 1, -1));

	for (unsigned int i = 0; i < points.size(); ++i)
	{
		points[i] = trans->transformPoint(points[i]);
	}
	return points;
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

vector<Vector3> OBB::getAllAxises(const OBB& obb) const
{
	auto aAxes = getAxises();
	auto bAxes = obb.getAxises();

	return {
			aAxes[0],
			aAxes[1],
			aAxes[2],
			bAxes[0],
			bAxes[1],
			bAxes[2],
			Vector3::cross(aAxes[0], bAxes[0]),
			Vector3::cross(aAxes[0], bAxes[1]),
			Vector3::cross(aAxes[0], bAxes[2]),
			Vector3::cross(aAxes[1], bAxes[0]),
			Vector3::cross(aAxes[1], bAxes[1]),
			Vector3::cross(aAxes[1], bAxes[2]),
			Vector3::cross(aAxes[2], bAxes[0]),
			Vector3::cross(aAxes[2], bAxes[1]),
			Vector3::cross(aAxes[2], bAxes[2])
	};
}

Projection OBB::project(const Vector3& axis) const
{
	auto vertices = getPoints();
	float min = vertices[0].dot(axis);
	float max = min;
	for (int i = 1; i < vertices.size(); i++) {
		// the axis must be normalized to get accurate projections
		float p = vertices[i].dot(axis);
		min = std::min(p, min);
		max = std::max(p, max);
	}

	return Projection(min,max);
}
