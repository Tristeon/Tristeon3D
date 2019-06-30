#pragma once
#include "Core/Components/Component.h"
#include "Editor/TypeRegister.h"
#include "Misc/vector.h"
#include "Math/Vector3.h"

namespace Leon
{
	class CollisionTesting : public Tristeon::Core::Components::Component
	{
	public:
		nlohmann::json serialize() override;
		void onGUI() override;

	private:
		bool checkCollision(Tristeon::Core::GameObject* a, Tristeon::Core::GameObject* b);
		bool hasOverlap(Tristeon::vector<Tristeon::Vector3> axes, Tristeon::vector<Tristeon::Vector3> aVerts, Tristeon::vector<Tristeon::Vector3> bVerts);

		Tristeon::vector<Tristeon::Vector3> getPoints(Tristeon::Core::GameObject * obj);
		Tristeon::vector<Tristeon::Vector3> getAxises(Tristeon::Core::GameObject* obj);

		Tristeon::Vector3 transformPoint(Tristeon::Core::GameObject* obj, Tristeon::Vector3 point);
		REGISTER_TYPE_H(CollisionTesting)
	};
}