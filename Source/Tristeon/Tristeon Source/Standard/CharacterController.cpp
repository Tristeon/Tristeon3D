#include "CharacterController.h"
#include "Core/Transform.h"
#include "Misc/Hardware/Keyboard.h"
#include "Misc/Hardware/Time.h"

namespace Tristeon
{
	namespace Standard
	{
		nlohmann::json CharacterController::serialize()
		{
			nlohmann:nlohmann::json j;
			j["typeID"] = typeid(CharacterController).name();
			j["speed"] = speed;
			return j;
		}

		void CharacterController::deserialize(nlohmann::json json)
		{
			speed = json["speed"];
		}

		void CharacterController::update()
		{
			float const sp = speed * Misc::Time::getDeltaTime();

			Math::Vector3 translation;
			if (Misc::Keyboard::getKey(Misc::KeyCode::W))
				translation.z -= sp;
			if (Misc::Keyboard::getKey(Misc::KeyCode::S))
				translation.z += sp;

			if (Misc::Keyboard::getKey(Misc::KeyCode::A))
				translation.x -= sp;
			if (Misc::Keyboard::getKey(Misc::KeyCode::D))
				translation.x += sp;

			transform->translate(translation);
		}
	}
}