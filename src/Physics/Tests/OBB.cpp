#include "OBB.h"
#include "Projection.h"
#include "Core/Rendering/DebugDrawManager.h"

using namespace Tristeon;
using namespace Tristan;

#include "Core/Transform.h"

REGISTER_TYPE_CPP(OBB)


void OBB::start()
{
}

void OBB::onGUI()
{
	Core::Rendering::DebugDrawManager::addCube(transform.get()->position.get(), size, transform.get()->rotation.get().eulerAngles(), 4, color);
}

bool OBB::collides(OBB* obb)
{
	vector<Vector3> allAxises = getAllAxises(obb);

	for (Vector3 axis : allAxises)
	{
		Projection p1 = project(axis);
		Projection p2 = obb->project(axis);

		if (!p1.overlap(p2))
			return false;
	}
	return true;
}

vector<Vector3> OBB::getAxises()
{
	Core::Transform* trans = transform.get();
	vector<Vector3> output;
	output.push_back(trans->right());
	output.push_back(trans->up());
	output.push_back(trans->forward());
	return output;
}

vector<Vector3> OBB::getPoints()
{
	vector<Vector3> points;
	const auto point = size / 2;
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
		const glm::vec4 transformedPoint = transform.get()->getTransformationMatrix() * glm::vec4(points[i].x, points[i].y, points[i].z, 1);
		points[i] = Vector3(transformedPoint.x, transformedPoint.y, transformedPoint.z);
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

vector<Vector3> OBB::getAllAxises(OBB* obb)
{
	auto aAxes = getAxises();
	auto bAxes = obb->getAxises();

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

Projection OBB::project(Vector3& axis)
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

	return Projection(min, max);
}
