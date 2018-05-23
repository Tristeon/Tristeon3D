#include "Camera.h"
#include "Core/GameObject.h"
#include "Core/Transform.h"
#include "Core/ManagerProtocol.h"
#include <Math/Vector3.h>
#include "Core/Message.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Core/Rendering/RenderManager.h"

#include <XPlatform/typename.h>

namespace Tristeon
{
	namespace Core
	{
		namespace Components
		{
			REGISTER_TYPE_CPP(Camera)

			void Camera::init()
			{
				if (!registered)
					ManagerProtocol::sendMessage({ MT_CAMERA_REGISTER, this });
				Component::init();
			}

			Camera::~Camera()
			{
				//Deregister
				if (registered)
					ManagerProtocol::sendMessage({ MT_CAMERA_DEREGISTER, this });
			}

			void Camera::setSkybox(std::string path)
			{
				skyboxPath = path;
				skybox = Rendering::RenderManager::getSkybox(path);
			}

			Rendering::Skybox* Camera::getSkybox() const
			{
				return skybox;
			}

			glm::mat4 Camera::getViewMatrix()
			{
				return getViewMatrix(gameObject.get()->transform.get());
			}

			glm::mat4 Camera::getProjectionMatrix(float aspect) const
			{
				return glm::perspective<float>(glm::radians(_fov), aspect, _nearClippingPlane, _farClippingPlane);
			}

			glm::mat4 Camera::getViewMatrix(Transform* t)
			{
				//Get parent matrix
				glm::mat4 par = glm::mat4(1.0f);
				if (t->getParent() != nullptr)
					par = getViewMatrix(t->getParent());

				//Get transformation
				glm::mat4 const tran = translate(glm::mat4(1.0f), Vec_Convert3(t->position.get()));
				glm::mat4 const rot = glm::mat4(t->rotation.get().getGLMQuat());

				//TODO: Parent calculation untested
				glm::mat4 const r = (tran * rot) * par;
				return inverse(r);
			}

			glm::mat4 Camera::getProjectionMatrix(float aspect, float fov, float near, float far)
			{
				return glm::perspective<float>(glm::radians(fov), aspect, near, far);
			}

			nlohmann::json Camera::serialize()
			{
				nlohmann::json output;
				output["typeID"] = TRISTEON_TYPENAME(Camera);
				output["fov"] = _fov;
				output["nearClippingPlane"] = _nearClippingPlane;
				output["farClippingPlane"] = _farClippingPlane;
				output["skybox"] = skyboxPath;
				return output;
			}

			void Camera::deserialize(nlohmann::json json)
			{
				_fov = json["fov"];
				_nearClippingPlane = json["nearClippingPlane"];
				_farClippingPlane = json["farClippingPlane"];

				const std::string skyVal = json["skybox"];
				if (skyVal != skyboxPath)
					skybox = Rendering::RenderManager::getSkybox(skyVal);
				skyboxPath = skyVal;
			}
		}
	}
}
