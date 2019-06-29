#pragma once
#include <vector>
#include "Core/Components/Component.h"
#include "Core/Transform.h"
#include "Projection.h"

namespace Tristan
{
	class OBB : public Tristeon::Core::Components::Component
	{
	public:
		void start() override;

		nlohmann::json serialize() override;
		void deserialize(nlohmann::json json) override;


		bool collides(OBB obb) const;
		Tristeon::vector<Tristeon::Vector3> getAxises() const;
	private:
		Tristeon::Vector3 size;
		Tristeon::Core::Transform* trans = nullptr;

		Tristeon::vector<Tristeon::Vector3> getAllAxises(const OBB& obb) const;
		Projection project(const Tristeon::Vector3& axis) const;
		Tristeon::vector<Tristeon::Vector3> getPoints() const;

		REGISTER_TYPE_H(OBB)
	};
}
