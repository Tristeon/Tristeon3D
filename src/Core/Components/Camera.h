#pragma once
#include "Component.h"
#include <glm/mat4x3.hpp>
#include "Editor/TypeRegister.h"
#include "Core/Rendering/Skybox.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Components
		{
			/**
			 * A camera is a device that is used by the renderer to display the 3D world
			 */
			class Camera : public Component
			{
			public:
				/**
				 * Initializes the camera and registers the camera to the rendering system
				 */
				void init() override;
				/**
				 * Deregisters the camera
				 */
				~Camera() override;
				
				float fov = 60;
				float nearClippingPlane = 0.3f;
				float farClippingPlane = 1000.0f;

				/**
				* Defines if the camera is rendered to the screen or not
				*/
				SimpleReadOnlyProperty(Camera, offscreen, bool)

				void setSkybox(std::string path);
				Rendering::Skybox* getSkybox() const;

				/**
				 * Returns the view matrix of this camera
				 * \return The view matrix
				 */
				glm::mat4 getViewMatrix();
				/**
				 * Returns the projection matrix of this camera
				 * \param aspect The aspect ratio of the screen
				 * \return The projection matrix
				 */
				glm::mat4 getProjectionMatrix(float aspect) const;

				/**
				 * Returns the view matrix based on transform t
				 * \param t The transform of the "camera"
				 * \return Returns a view matrix
				 */
				static glm::mat4 getViewMatrix(Transform* t);
				/**
				 * Returns a projection matrix
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
				std::string skyboxPath = "";
				Rendering::Skybox* skybox = nullptr;

				REGISTER_TYPE_H(Camera)
			};
		}
	}
}
