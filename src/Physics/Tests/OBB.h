#pragma once
#include "Core/Components/Component.h"
#include "Core/Transform.h"
#include "Projection.h"
#include "Misc/Color.h"

namespace Tristan
{
	class OBB : public Tristeon::Core::Components::Component
	{
	public:
		void start() override;
		void onGUI() override;

		nlohmann::json serialize() override;
		void deserialize(nlohmann::json json) override;

		bool collides(OBB* obb);
		Tristeon::vector<Tristeon::Vector3> getAxises();

		Tristeon::Misc::Color color;
	private:
		Tristeon::Vector3 size = Tristeon::Vector3::one;

		Tristeon::vector<Tristeon::Vector3> getAllAxises(OBB* obb);
		Projection project(Tristeon::Vector3& axis);
		Tristeon::vector<Tristeon::Vector3> getPoints();

		REGISTER_TYPE_H(OBB)
	};
}
