#include "FirstPersonCameraController.h"
#include "Core/Transform.h"
#include "Misc/Hardware/Mouse.h"
#include "Misc/Hardware/Time.h"

namespace Tristeon
{
	namespace Standard
	{
		DerivedRegister<FirstPersonCameraController> FirstPersonCameraController::reg;

		nlohmann::json FirstPersonCameraController::serialize()
		{
			nlohmann::json j;
			j["typeID"] = typeid(FirstPersonCameraController).name();
			j["sensitivity"] = sensitivity;
			return j;
		}

		void FirstPersonCameraController::deserialize(nlohmann::json json)
		{
			sensitivity = json["sensitivity"];
		}

		void FirstPersonCameraController::start()
		{
		}

		void FirstPersonCameraController::update()
		{
			float const x = Misc::Mouse::getMouseDelta().x * sensitivity * Misc::Time::getDeltaTime();
			float const y = Misc::Mouse::getMouseDelta().y * sensitivity * Misc::Time::getDeltaTime();
			xRot -= y;
			yRot -= x;

			transform.get()->rotation = Math::Quaternion::euler(xRot, yRot, 0);
		}
	}
}
