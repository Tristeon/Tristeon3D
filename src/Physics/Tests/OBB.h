#pragma once
#include <vector>
#include "Core/Components/Component.h"
#include "Core/Transform.h"

namespace Tristan
{
	class OBB : public Tristeon::Core::Components::Component
	{
	public:
		void start() override;

		nlohmann::json serialize() override;
		void deserialize(nlohmann::json json) override;

		bool collides(OBB obb);
		std::vector<Tristeon::Vector3> getAxises();
	private:
		Tristeon::Vector3 getPoints();
		Tristeon::Vector3 size;
		Tristeon::Core::Transform* trans = nullptr;

		REGISTER_TYPE_H(OBB)
	};
}
