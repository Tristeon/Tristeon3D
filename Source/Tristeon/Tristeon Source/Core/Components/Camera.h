#pragma once
#include "Component.h"
#include "Math/Rect.h"
#include <glm/mat4x3.hpp>
#include "Editor/TypeRegister.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Components
		{
			/**
			 * \brief A camera is a device that is used by the renderer to display the 3D world
			 */
			class Camera : public Component
			{
			public:
				/**
				 * \brief Initializes the camera and registers the camera to the rendering system
				 */
				void init() override;
				/**
				 * \brief Deregisters the camera
				 */
				~Camera() override;
				
				/**
				* \brief The field of view of the camera
				*/
				Property(float, fov);
				SetProperty(fov) { _fov = value; }
				GetPropertyConst(fov) { return _fov; }
				
				/**
				* \brief The near clipping plane of the camera
				*/
				Property(float, nearClippingPlane);
				SetProperty(nearClippingPlane) { _nearClippingPlane = value; }
				GetPropertyConst(nearClippingPlane) { return _nearClippingPlane; }

				/**
				* \brief The far clipping plane of the camera
				*/
				Property(float, farClippingPlane);
				SetProperty(farClippingPlane) { _farClippingPlane = value; }
				GetPropertyConst(farClippingPlane) { return _farClippingPlane; }

				/**
				* \brief The screen rect of the camera
				*/
				Property(Math::Rect, rect);
				SetProperty(rect) { _rect = value; }
				GetPropertyConst(rect) { return _rect; }

				/**
				* \brief Defines if the camera is rendered to the screen or not
				*/
				ReadOnlyProperty(bool, offscreen);
				GetPropertyConst(offscreen) { return _offscreen; }

				/**
				 * \brief Returns the view matrix of this camera
				 * \return The view matrix
				 */
				glm::mat4 getViewMatrix() const;
				/**
				 * \brief Returns the projection matrix of this camera
				 * \param aspect The aspect ratio of the screen
				 * \return The projection matrix
				 */
				glm::mat4 getProjectionMatrix(float aspect) const;

				/**
				 * \brief Returns the view matrix based on transform t
				 * \param t The transform of the "camera"
				 * \return Returns a view matrix
				 */
				static glm::mat4 getViewMatrix(Transform* t);
				/**
				 * \brief Returns a projection matrix
				 * \param aspect The aspection ratio of the screen
				 * \param fov The field of view
				 * \param near The near clipping plane
				 * \param far The far clipping plane
				 * \return Returns the projection matrix
				 */
				static glm::mat4 getProjectionMatrix(float aspect, float fov, float near, float far);

				nlohmann::json serialize() override;

				void deserialize(nlohmann::json json) override;
			private:
				/**
				 * \brief The field of view of the camera
				 */
				float _fov = 60;
				/**
				 * \brief The near clipping plane of the camera
				 */
				float _nearClippingPlane = 0.3f;
				/**
				 * \brief The far clipping plane of the camera
				 */
				float _farClippingPlane = 1000.0f;
				/**
				 * \brief Defines if the camera is rendered ot the screen or not
				 */
				bool _offscreen = false;
				/**
				 * \brief The onscreen rect of the camera
				 */
				Math::Rect _rect = Math::Rect(-1, -1, 2, 2);

				REGISTER_TYPE_H(Camera)
			};
		}
	}
}
