#include "CollisionTestingLeon.h"
#include "Misc/Hardware/Keyboard.h"
#include "Core/Rendering/DebugDrawManager.h"
#include <Misc/Hardware/Time.h>

REGISTER_TYPE_CPP(CollisionTestingLeon)
using namespace Tristeon;
using namespace Tristeon::Misc;

nlohmann::json CollisionTestingLeon::serialize()
{
	nlohmann::json j;

	return j;
}

void CollisionTestingLeon::deserialize(nlohmann::json json)
{

}

void CollisionTestingLeon::start()
{

}

void CollisionTestingLeon::update()
{
	float movementX = Keyboard::getKey(D) - Keyboard::getKey(A);
	float movementZ = Keyboard::getKey(W) - Keyboard::getKey(S);
	float movementY = Keyboard::getKey(Q) - Keyboard::getKey(E);

	float rotationX = Keyboard::getKey(I) - Keyboard::getKey(K);
	float rotationZ = Keyboard::getKey(O) - Keyboard::getKey(U);
	float rotationY = Keyboard::getKey(L) - Keyboard::getKey(J);

	if (Keyboard::getKeyDown(KeyCode::NUM_1))
		movingID = 1;
	if (Keyboard::getKeyDown(KeyCode::NUM_2))
		movingID = 2;

	if (movingID == 1)
	{
		colliderA.position += Vector3(movementX, movementY, movementZ) * Time::getDeltaTime();
		colliderA.rotation += Vector3(rotationX, rotationY, rotationZ) * Time::getDeltaTime();
	}
}

void CollisionTestingLeon::onGUI()
{
	Misc::Color const color = checkOBBonOBB() ? Color(1, 0, 0, 1) : Color(0, 1, 0, 0);

	Core::Rendering::DebugDrawManager::addCube(colliderA.position, colliderA.size, colliderA.rotation, 2, color);
	Core::Rendering::DebugDrawManager::addCube(colliderB.position, colliderB.size, colliderB.rotation, 2, color);
}

bool CollisionTestingLeon::checkOBBonOBB()
{

	return true;
}
