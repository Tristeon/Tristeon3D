#pragma once
#include "TObject.h"
#include "Math/Vector3.h"
#include "Misc/Property.h"
#include "Editor/TypeRegister.h"
#include "Misc/vector.h"
#include <glm/mat4x4.hpp>
#include "Math/Quaternion.h"

namespace Tristeon {
	namespace Scenes {
		class SceneManager;
	}
}

namespace Tristeon
{
	namespace Core
	{
		/**
		 * \brief Transform is a class used to describe the transform, rotation and scale of an object. 
		 * It's most commonly used by GameObject. Every GameObject will always have a Transform at any time.
		 * Transform also describes parent-child relationships between gameobjects.
		 */
		class Transform final : public TObject
		{
			friend Scenes::SceneManager;
		public:
			/**
			 * \brief Cleans up transform and all of its relationships
			 */
			~Transform();

			/**
			* \brief The global position of this transform
			*/
			Property(Math::Vector3, position);
			GetPropertyConst(position) { return getGlobalPosition(); }
			SetProperty(position) { setGlobalPosition(value); }

			/**
			* \brief The local position of this transform
			*/
			Property(Math::Vector3, localPosition);
			GetPropertyConst(localPosition) { return _localPosition; }
			SetProperty(localPosition) { _localPosition = value; }

			/**
			* \brief The global scale of this transform
			*/
			Property(Math::Vector3, scale);
			GetPropertyConst(scale) { return getGlobalScale(); }
			SetProperty(scale) { setGlobalScale(value); }

			/**
			* \brief The local scale of this transform
			*/
			Property(Math::Vector3, localScale);
			GetPropertyConst(localScale) { return _localScale; }
			SetProperty(localScale) { _localScale = value; }

			/**
			* \brief The local rotation of this transform
			*/
			Property(Math::Quaternion, localRotation);
			GetPropertyConst(localRotation) { return _localRotation; }
			SetProperty(localRotation) { _localRotation = value; }

			/**
			* \brief The global rotation of this transform
			*/
			Property(Math::Quaternion, rotation);
			GetPropertyConst(rotation) { return getGlobalRotation(); }
			SetProperty(rotation) { setGlobalRotation(value); }
			
			/**
			 * \brief Sets the parent of this transform
			 * \param parent The parent
			 * \param keepWorldTransform Defines wether or not our world transformation should be kept 
			 */
			void setParent(Transform* parent, bool keepWorldTransform = true);
			/**
			 * \brief Gets the parent of this transform
			 * \return Returns the parent
			 */
			Transform* getParent() const;

			/**
			 * \brief Serializes this transform into a json file
			 * \return The json object
			 */
			nlohmann::json serialize() override;
			/**
			 * \brief Deserializes this transform from a json file
			 * \param json The json file
			 */
			void deserialize(nlohmann::json json) override;

			/**
			 * \brief Returns a transformation matrix based on the position, scale, rotation and parent of this transform
			 * \return Returns a transformation matrix
			 */
			glm::mat4 getTransformationMatrix() const;

			/**
			 * \brief Rotates around axis [axis] with rotation [rot]
			 * \param axis The axis to be rotated around
			 * \param rot The amount of rotation in degrees
			 */
			void rotate(Math::Vector3 axis, float rot);

			/**
			 * \brief Translates the transform in local space
			 * \param t The amount of translation
			 */
			void translate(Math::Vector3 t);

			/**
			* \brief Translates the transform in local space
			* \param t The amount of translation
			*/
			void translate(float x, float y, float z);

			/**
			 * \brief Transforms a given point from local to global space
			 * \param point The position to be transformed
			 * \return The transformed position
			 */
			Math::Vector3 transformPoint(Math::Vector3 point) const	;
			/**
			 * \brief Transforms a given point from global to local space
			 * \param point The position to be transformed
			 * \return The transformed position
			 */
			Math::Vector3 inverseTransformPoint(Math::Vector3 point) const;

			void lookAt(Transform* target, Math::Vector3 worldUp = Math::Vector3::up);

			/**
			* \brief The green axis of the transform in world space.
			*/
			Math::Vector3 up() const;
			
			/**
			 * \brief The red axis of the transform in world space.
			 */
			Math::Vector3 right() const;

			/**
			 * \brief The blue axis of the transform in world space.
			 */
			Math::Vector3 forward() const;

		private:
			/**
			 * \brief Gets the global position of this transform
			 * \return Returns the global position
			 */
			Math::Vector3 getGlobalPosition() const;
			/**
			 * \brief Sets the global position of this transform
			 * \param pos The global position
			 */
			void setGlobalPosition(Math::Vector3 pos);

			/**
			 * \brief Gets the global scale of this transform
			 * \return Returns the global scale
			 */
			Math::Vector3 getGlobalScale() const;
			/**
			 * \brief Sets the global scale of this transform
			 * \param scale The global scale
			 */
			void setGlobalScale(Math::Vector3 scale);

			/**
			 * \brief Gets the global rotation of this transform
			 * \return Returns the global rotation
			 */
			Math::Quaternion getGlobalRotation() const;
			/**
			 * \brief Sets the global rotation of this transform
			 * \param rot The rotation
			 */
			void setGlobalRotation(Math::Quaternion rot);

			/**
			 * \brief The local position of this transform
			 */
			Math::Vector3 _localPosition = Math::Vector3(0, 0, 0);
			/**
			 * \brief The local scale of this transform
			 */
			Math::Vector3 _localScale = Math::Vector3(1, 1, 1);
			/**
			 * \brief The local rotation of this transform
			 */
			Math::Quaternion _localRotation = Math::Quaternion();

			/**
			 * \brief The parent of this transform
			 */
			Transform* parent = nullptr;

			/**
			 * \brief The id of the parent
			 */
			std::string parentID = "null";

			/**
			 * \brief The children of this transform
			 */
			Tristeon::vector<Transform*> children;

			REGISTER_TYPE_H(Transform)
		};
	}
}
