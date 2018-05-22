#include "TestScript.h"
#include "Math/Quaternion.h"
#include "Core/Transform.h"
#include "Misc/Hardware/Keyboard.h"
#include "Misc/Hardware/Time.h"

DerivedRegister<TestScript> TestScript::reg;

void TestScript::update()
{
	transform.get()->rotate(Math::Vector3(0, 1, 0), speed * Misc::Time::getDeltaTime());
}

nlohmann::json TestScript::serialize()
{
	nlohmann::json output;
	output["typeID"] = typeid(TestScript).name();
	output["speed"] = speed;
	return output;
}

void TestScript::deserialize(nlohmann::json json)
{
	speed = json["speed"];
}