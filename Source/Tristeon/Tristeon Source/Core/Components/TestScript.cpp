#include "TestScript.h"
#include "Math/Quaternion.h"
#include "Core/Transform.h"

DerivedRegister<TestScript> TestScript::reg;

void TestScript::update()
{
	static float test = 0;
	test += 0.08f;
	transform->position = Math::Vector3::back * 8 * Math::Quaternion::euler(test, 0, 0);
	transform->rotation = Math::Quaternion::lookRotation(transform->position, Math::Vector3());
}

nlohmann::json TestScript::serialize()
{
	nlohmann::json output;
	output["typeID"] = typeid(TestScript).name();
	return output;
}
