#include "CollisionTesting.h"
#include "OBB.h"
#include "Core/GameObject.h"
#include "Core/Rendering/Components/MeshRenderer.h"

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

void CollisionTesting::onGUI()
{
	auto cube1 = Core::GameObject::find("Cube 1");
	auto cube2 = Core::GameObject::find("Cube 2");
	if (cube1 != nullptr)
		a = cube1->getComponent<OBB>();
	else return;
	if (cube2 != nullptr)
		b = cube2->getComponent<OBB>();
	else return;

	if (a == nullptr || b == nullptr) return;

	if (a->collides(b))
	{
		a->color = Misc::Color(1, 0, 0);
		b->color = Misc::Color(1, 0, 0);
	}
	else
	{
		a->color = Misc::Color(0, 0, 1);
		b->color = Misc::Color(0, 0, 1);
	}
}
