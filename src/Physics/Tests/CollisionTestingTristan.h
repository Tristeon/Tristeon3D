#pragma once
#include "Core/Components/Component.h"
#include "Editor/TypeRegister.h"

namespace Tristan
{
	class CollisionTesting final : public Tristeon::Core::Components::Component
	{
	public:
		CollisionTesting();
		nlohmann::json serialize() override;
		void deserialize(nlohmann::json json) override;
		void start() override;
		void update() override;
		void onGUI() override;

	private:
		REGISTER_TYPE_H(CollisionTesting)
	};
}
