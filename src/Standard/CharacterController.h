#pragma once
#include "Core/Components/Component.h"
#include <Editor/TypeRegister.h>

namespace Tristeon
{
	namespace Standard
	{
		class CharacterController : public Core::Components::Component
		{
		public:
			nlohmann::json serialize() override;
			void deserialize(nlohmann::json json) override;
			void update() override;
		private:
			float speed = 10;

			REGISTER_TYPE_H(CharacterController);
		};
	}
}
