#pragma once
#include "TObject.h"
#include "Math/Vector3.h"
#include "Misc/Property.h"
#include "Editor/TypeRegister.h"
#include "Misc/vector.h"
#include <glm/mat4x4.hpp>
#include "Math/Quaternion.h"

namespace Tristeon::Core
{
	/**
	 * Transform is a class used to describe the translation, rotation and scale of an object.
	 * It's usually contained by GameObject, although its usage is not limited to GameObjects.
	 * Transform also describes parent-child relationships.
	 */
	class Transform final : public TObject
	{
		friend class SceneManager;
	public:
		~Transform();

		/**
		 * The global position of this transform.
		 * Warning: This value is currently not cached and will do multiple matrix calculations,
		 * and might be an expensive operation depending on the hierarchy depth.
		 */
		Property(Transform, position, Math::Vector3);
		GetProperty(position) { return getGlobalPosition(); }
		SetProperty(position) { setGlobalPosition(value); }

		Property(Transform, localPosition, Math::Vector3);
		GetProperty(localPosition) { return _localPosition; }
		SetProperty(localPosition) { _localPosition = value; }

		/**
		 * The global scale of this transform.
		 * Warning: This value is currently not cached and will do multiple matrix calculations,
		 * and might be an expensive operation depending on the hierarchy depth.
		 */
		Property(Transform, scale, Math::Vector3);
		GetProperty(scale) { return getGlobalScale(); }
		SetProperty(scale) { setGlobalScale(value); }

		Property(Transform, localScale, Math::Vector3);
		GetProperty(localScale) { return _localScale; }
		SetProperty(localScale) { _localScale = value; }

		/**
		 * The global rotation of this transform.
		 * Warning: This value is currently not cached and will do multiple matrix calculations,
		 * and might be an expensive operation depending on the hierarchy depth.
		 */
		Property(Transform, rotation, Math::Quaternion);
		GetProperty(rotation) { return getGlobalRotation(); }
		SetProperty(rotation) { setGlobalRotation(value); }

		Property(Transform, localRotation, Math::Quaternion);
		GetProperty(localRotation) { return _localRotation; }
		SetProperty(localRotation) { _localRotation = value; }

		/**
		 * Sets the parent of this transform. Use nullptr to remove the current parent relationship.
		 * \param keepWorldTransform Defines wether or not our world transformation should be kept.
		 */
		void setParent(Transform* parent, bool keepWorldTransform = true);

		/**
		 * Gets the current parent. Nullptr if this transform has no parent.
		 */
		Transform* getParent() const;

		/**
		 * Returns a transformation matrix based on the position, scale, rotation and parent hierarchy of this transform.
		 * Warning: Transformation matrices aren't cached and might do multiple recursive matrix calculations,
		 * which might turn this function into an expensive operation depending on the hierarchy depth.
		 */
		glm::mat4 getTransformationMatrix();

		/**
		 * Globally rotates around axis [axis] with rotation [rot]
		 */
		void rotate(Math::Vector3 axis, float rot);

		/**
		 * Translates the transform in local space.
		 */
		void translate(Math::Vector3 t);

		/**
		 * Translates the transform in local space.
		 */
		void translate(float x, float y, float z);

		/**
		 * Transforms a given point from local to global space.
		 * Warning: This function will do multiple recursive matrix calculations,
		 * and might be an expensive operation depending on the hierarchy depth.
		 */
		Math::Vector3 transformPoint(Math::Vector3 point);
		/**
		 * Transforms a given point from global to local space
		 * Warning: This function will do multiple recursive matrix calculations,
		 * and might be an expensive operation depending on the hierarchy depth.
		 */
		Math::Vector3 inverseTransformPoint(Math::Vector3 point);

		/**
		 * Sets the global rotation so that the forward vector points directly at the target.
		 */
		void lookAt(Transform* target, Math::Vector3 worldUp = Math::Vector3::up);

		/**
		 * The green axis of the transform in world space.
		 */
		Math::Vector3 up();

		/**
		 * The red axis of the transform in world space.
		 */
		Math::Vector3 right();

		/**
		 * The blue axis of the transform in world space.
		 */
		Math::Vector3 forward();

		nlohmann::json serialize() override;
		void deserialize(nlohmann::json json) override;
	private:
		Math::Vector3 getGlobalPosition();
		void setGlobalPosition(Math::Vector3 pos);
		Math::Vector3 getGlobalScale();
		void setGlobalScale(Math::Vector3 scale);
		Math::Quaternion getGlobalRotation();
		void setGlobalRotation(Math::Quaternion rot);

		Math::Vector3 _localPosition = { 0, 0, 0 };
		Math::Vector3 _localScale = { 1, 1, 1 };
		Math::Quaternion _localRotation = {};

		/**
		 * The id of the parent. Used to assign parent child relationships through a lookup in the scene.
		 */
		std::string parentID = "null";

		Transform* parent = nullptr;
		Tristeon::vector<Transform*> children;

		REGISTER_TYPE_H(Transform)
	};
}