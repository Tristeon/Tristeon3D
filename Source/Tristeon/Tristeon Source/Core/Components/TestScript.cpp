#include "TestScript.h"
#include "Math/Quaternion.h"
#include "Core/Transform.h"
#include "Misc/Hardware/Keyboard.h"
#include "Misc/Hardware/Time.h"

DerivedRegister<TestScript> TestScript::reg;

void TestScript::update()
{
	if (!started && Misc::Keyboard::getKeyDown(Misc::ENTER))
		started = true;
	
	if (!started)
		return;

	t += speed * Misc::Time::getDeltaTime();
	if (t >= 1 || t <= 0)
		speed *= -1;

	transform->position = Math::Vector3::lerp(start, end, t);
}

nlohmann::json TestScript::serialize()
{
	nlohmann::json output;
	output["typeID"] = typeid(TestScript).name();
	output["speed"] = speed;
	output["start"] = start.serialize();
	output["end"] = end.serialize();
	return output;
}

void TestScript::deserialize(nlohmann::json json)
{
	speed = json["speed"];
	start.deserialize(json["start"]);
	end.deserialize(json["end"]);
}
