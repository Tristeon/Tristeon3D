#include "Camera.h"
#include "Core/GameObject.h"
#include "Core/Transform.h"
#include "Core/ManagerProtocol.h"
#include <Math/Vector3.h>
#include "Core/Message.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Tristeon
{
	namespace Core
	{
		namespace Components
		{
			REGISTER_TYPE_CPP(Camera)

			void Camera::init()
			{
				ManagerProtocol::sendMessage({ MT_CAMERA_REGISTER, this });
				//Normally gets set in the base class, but we're not calling Component::init, so we gotta set it ourselves
				registered = true;
			}

			Camera::~Camera()
			{
				//Deregister
				if (registered)
					ManagerProtocol::sendMessage({ MT_CAMERA_DEREGISTER, this });
			}

			glm::mat4 Camera::getViewMatrix() const
			{
				return getViewMatrix(gameObject->transform);
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
				glm::mat4 const tran = translate(glm::mat4(1.0f), Vec_Convert3(t->position));
				glm::mat4 const rot = glm::mat4(t->rotation.getGLMQuat());

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
				output["typeID"] = typeid(Camera).name();
				output["fov"] = _fov;
				output["nearClippingPlane"] = _nearClippingPlane;
				output["farClippingPlane"] = _farClippingPlane;
				return output;
			}

			void Camera::deserialize(nlohmann::json json)
			{
				_fov = json["fov"];
				_nearClippingPlane = json["nearClippingPlane"];
				_farClippingPlane = json["farClippingPlane"];
			}
		}
	}
}
