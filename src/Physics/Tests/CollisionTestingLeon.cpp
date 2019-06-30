#include "CollisionTestingLeon.h"
#include "OBB.h"
#include "Core/GameObject.h"
#include "Core/Rendering/Components/MeshRenderer.h"
#include "Core/Rendering/DebugDrawManager.h"

using namespace Tristeon;
using namespace Tristeon::Core;

namespace Leon
{
	REGISTER_TYPE_CPP(CollisionTesting)
		nlohmann::json CollisionTesting::serialize()
	{
		nlohmann::json j = Component::serialize();
		j["typeID"] = TRISTEON_TYPENAME(CollisionTesting);
		return j;
	}

	void CollisionTesting::onGUI()
	{
		GameObject* obj1 = GameObject::find("test_1");
		GameObject* obj2 = GameObject::find("test_2");
		if (obj1 == nullptr || obj2 == nullptr)
			return;

		bool const colliding = checkCollision(obj1, obj2);

		Misc::Color color = Misc::Color();
		if (colliding)
			color = Misc::Color(1, 0, 0, 1);

		Rendering::DebugDrawManager::addCube(obj1->transform.get()->position.get(), Vector3::one, obj1->transform.get()->rotation.get().eulerAngles(), 4, color);
		Rendering::DebugDrawManager::addCube(obj2->transform.get()->position.get(), Vector3::one, obj2->transform.get()->rotation.get().eulerAngles(), 4, color);
	}

	bool CollisionTesting::checkCollision(Tristeon::Core::GameObject* a, Tristeon::Core::GameObject* b)
	{
		vector<Vector3> aAxes = getAxises(a);
		vector<Vector3> bAxes = getAxises(b);
		vector<Vector3> combinedAxes =
		{
			aAxes[0],
			aAxes[1],
			aAxes[2],
			bAxes[0],
			bAxes[1],
			bAxes[2],
			aAxes[0].cross(bAxes[0]),
			aAxes[0].cross(bAxes[1]),
			aAxes[0].cross(bAxes[2]),
			aAxes[1].cross(bAxes[0]),
			aAxes[1].cross(bAxes[1]),
			aAxes[1].cross(bAxes[2]),
			aAxes[2].cross(bAxes[0]),
			aAxes[2].cross(bAxes[1]),
			aAxes[2].cross(bAxes[2])
		};

		return hasOverlap(combinedAxes, getPoints(a), getPoints(b));
	}

	bool CollisionTesting::hasOverlap(vector<Vector3> axes, vector<Vector3> aVerts, vector<Vector3> bVerts)
	{
		constexpr float minimum = -std::numeric_limits<float>::max();
		constexpr float maximum = std::numeric_limits<float>::max();

		//Iterate over all separating axises
		for (unsigned int i = 0; i < axes.size(); i++)
		{
			//Find minimium and maximum projection of both shapes on current separating axis
			float aMin = maximum, bMin = maximum;
			float aMax = minimum, bMax = minimum;

			if (axes[i] == Vector3::zero) //Handles cross products with parallel lines resulting in zero vectors
				continue;

			//Project all vertices of A onto axis[i]
			for (unsigned int j = 0; j < bVerts.size(); j++)
			{
				float p = Vector3::dot(aVerts[j], axes[i]);

				aMin = std::min(p, aMin);
				aMax = std::max(p, aMax);
			}

			//Project all vertices of B onto axis[i]
			for (unsigned int j = 0; j < bVerts.size(); j++)
			{
				float p = Vector3::dot(bVerts[j], axes[i]);

				bMin = std::min(p, bMin);
				bMax = std::max(p, bMax);
			}

			//Found separating axis
			if (aMin > bMax || bMin > aMax)
				return false;
		}
		return true;
	}

	Tristeon::vector<Tristeon::Vector3> CollisionTesting::getPoints(Tristeon::Core::GameObject* obj)
	{
		const auto topRight = Vector3::one / 2;

		vector<Vector3> points = {
			transformPoint(obj, topRight),							  //Top right forwards
			transformPoint(obj, topRight * Vector3(-1, 1, 1)),		  //Top left forwards
			transformPoint(obj, topRight * Vector3(-1, -1, 1)),		  //Bottom left forwards
			transformPoint(obj, topRight * Vector3(-1, -1, -1)),	  //Bottom left backwards
			transformPoint(obj, topRight * Vector3(1, -1, 1)),		  //Bottom right forwards
			transformPoint(obj, topRight * Vector3(1, 1, -1)),		  //Top right backwards
			transformPoint(obj, topRight * Vector3(1, -1, -1)),		  //Bottom right backwards
			transformPoint(obj, topRight * Vector3(-1, 1, -1))		  //Bottom left backwards
		};

		return points;
	}

	Tristeon::vector<Tristeon::Vector3> CollisionTesting::getAxises(Tristeon::Core::GameObject * obj)
	{
		return {
			obj->transform.get()->right(),
			obj->transform.get()->up(),
			obj->transform.get()->forward()
		};
	}

	Tristeon::Vector3 CollisionTesting::transformPoint(Tristeon::Core::GameObject* obj, Tristeon::Vector3 point)
	{
		//Transform local point into global space
		const glm::vec4 result = obj->transform.get()->getTransformationMatrix() * glm::vec4(point.x, point.y, point.z, 1);
		return { result.x, result.y, result.z };
	}
}