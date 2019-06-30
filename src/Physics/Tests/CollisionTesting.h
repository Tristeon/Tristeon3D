#pragma once
#include "Core/Components/Component.h"
#include "Editor/TypeRegister.h"
#include "OBB.h"

namespace Tristan
{
	class CollisionTesting : public Tristeon::Core::Components::Component
	{
	public:
		CollisionTesting();
		nlohmann::json serialize() override;
		void deserialize(nlohmann::json json) override;
		void start() override;
		void update() override;
		void onGUI() override;

	private:
		OBB* a;
		OBB* b;

		REGISTER_TYPE_H(CollisionTesting)
	};
}
