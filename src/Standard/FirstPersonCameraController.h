#pragma once
#include "Core/Components/Component.h"
#include "Core/TypeRegister.h"

namespace Tristeon
{
	namespace Standard
	{
		class FirstPersonCameraController : public Core::Components::Component
		{
		public:
			REGISTER_TYPE_H(FirstPersonCameraController)

			nlohmann::json serialize() override;
			void deserialize(nlohmann::json json) override;
			void start() override;
			void update() override;
		private:
			float sensitivity = 10;

			float xRot = 0;
			float yRot = 0;
		};
	}
}
