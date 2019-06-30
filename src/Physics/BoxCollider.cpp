#include "BoxCollider.h"
#include "Physics.h"
#include "Core/Transform.h"
#include "Core/GameObject.h"
#include "Core/Rendering/DebugDrawManager.h"

namespace Tristeon
{
	namespace Physics
	{
		ColliderData::ColliderData(BoxCollider* collider) : aabb(collider->getAABB()), collider(collider), gameObject(collider->gameObject.get())
		{
		}

		ColliderData::ColliderData(AABB aabb, BoxCollider* collider) : aabb(aabb), collider(collider), gameObject(collider->gameObject.get())
		{
		}

		REGISTER_TYPE_CPP(BoxCollider)

		bool BoxCollider::AABBvsAABB(const AABB& a) const
		{
			//Exit with no intersection if found separated along an axis
			if (aabb.max.x < a.min.x || aabb.min.x > a.max.x) return false;
			if (aabb.max.y < a.min.y || aabb.min.y > a.max.y) return false;
			if (aabb.max.z < a.min.z || aabb.min.z > a.max.z) return false;

			// No separating axis found, therefor there is at least one overlapping axis
			return true;
		}

		bool BoxCollider::intersect(const BoxCollider& collider) const
		{
			return AABBvsAABB(collider.aabb);
		}

		BoxCollider::BoxCollider() : isTrigger(false), bounciness(0), friction(0)
		{
			size = Vector3::one;
			center = Vector3::zero;
			isTrigger = false;
			bounciness = 0;
			ignoreRaycast = false;
		}

		BoxCollider::BoxCollider(Core::GameObject* obj)
		{
			size = Vector3::one;
			center = Vector3::zero;
			isTrigger = false;
			bounciness = 0;
			friction = 0;
		}

		BoxCollider::BoxCollider(const Vector3& center, const Vector3& size) : center(center), size(size)
		{
			isTrigger = false;
			bounciness = 0;
			friction = 0;
		}

		BoxCollider::BoxCollider(const Vector3& center, const Vector3& size, const float& restitution)
			: center(center), size(size), bounciness(restitution)
		{
			isTrigger = false;
			friction = 0;
		}

		BoxCollider::~BoxCollider()
		{
			if (Physics::instance != nullptr)
				Physics::instance->remove(this);
		}

		void BoxCollider::start()
		{
			updateAABB();
		}

		bool BoxCollider::pointvsAABB(const Vector3& point) const
		{
			if (point.x < aabb.min.x || point.x > aabb.max.x) return false;
			if (point.y < aabb.min.y || point.y > aabb.max.y) return false;
			if (point.z < aabb.min.z || point.z > aabb.max.z) return false;
			return true;
		}

		Vector3 BoxCollider::getPosition() const
		{
			return aabb.getPosition();
		}

		std::vector<Vector3> BoxCollider::getExtremes() const
		{
			std::vector<Vector3> extremes;
			extremes.push_back(Vector3(aabb.min.x, aabb.min.y, aabb.min.z));
			extremes.push_back(Vector3(aabb.max.x, aabb.min.y, aabb.min.z));
			extremes.push_back(Vector3(aabb.max.x, aabb.max.y, aabb.min.z));
			extremes.push_back(Vector3(aabb.min.x, aabb.max.y, aabb.min.z));
			extremes.push_back(Vector3(aabb.min.x, aabb.min.y, aabb.max.z));
			extremes.push_back(Vector3(aabb.max.x, aabb.min.y, aabb.max.z));
			extremes.push_back(Vector3(aabb.max.x, aabb.max.y, aabb.max.z));
			extremes.push_back(Vector3(aabb.min.x, aabb.max.y, aabb.max.z));

			return extremes;
		}

		void BoxCollider::init()
		{
			Component::init();

			updateAABB();

			Physics::instance->addCollider(this);
		}

		void BoxCollider::updateAABB()
		{
			Vector3 const size = this->size * transform.get()->scale;
			aabb.min = transform.get()->position.get() + center - size / 2;
			aabb.max = transform.get()->position.get() + center + size / 2;
		}

		void BoxCollider::onGUI()
		{
			//if (isTrigger)
			//	Core::Rendering::DebugDrawManager::addCube(aabb.min, aabb.max, 2, Misc::Color(1, 0, 0, 1));
			//else 
			//	Core::Rendering::DebugDrawManager::addCube(aabb.min, aabb.max, 2, Misc::Color(0, 1, 0, 1));
		}

		void BoxCollider::update()
		{
			updateAABB();
		}

		nlohmann::json BoxCollider::serialize()
		{
			nlohmann::json j;
			j["typeID"] = TRISTEON_TYPENAME(BoxCollider);
			j["name"] = "BoxCollider";
			j["center"] = center;
			j["size"] = size;
			j["bounciness"] = bounciness;
			j["friction"] = friction;
			j["isTrigger"] = isTrigger;
			j["ignoreRaycast"] = ignoreRaycast;
			return j;
		}

		void BoxCollider::deserialize(nlohmann::json json)
		{
			center = json["center"];
			size = json["size"];
			bounciness = json["bounciness"];
			friction = json["friction"];
			isTrigger = json["isTrigger"];
			ignoreRaycast = json["ignoreRaycast"];
			updateAABB();
		}
	}
}
