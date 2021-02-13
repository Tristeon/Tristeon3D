#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Core/Transform.h>
#include <Core/Collector.h>
#include <Core/GameObject.h>
#include <Core/TypeRegister.h>

#include <Data/Resources.h>
#include <Math/Vector3.h>

namespace Tristeon::Core::Components
{
	REGISTER_TYPE_CPP(Camera);

	Camera::Camera()
	{
		Collector<Camera>::add(this);
	}

	Camera::~Camera()
	{
		Collector<Camera>::remove(this);
	}

	void Camera::setSkybox(std::string path)
	{
		skyboxPath = path;
		skybox = Data::Resources::jsonLoad<Rendering::Skybox>(path);
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
		return glm::perspective<float>(glm::radians(fov), aspect, nearClippingPlane, farClippingPlane);
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
		output["typeID"] = Type<Camera>::fullName();
		output["fov"] = fov;
		output["nearClippingPlane"] = nearClippingPlane;
		output["farClippingPlane"] = farClippingPlane;
		output["skybox"] = skyboxPath;
		return output;
	}

	void Camera::deserialize(nlohmann::json json)
	{
		fov = json["fov"];
		nearClippingPlane = json["nearClippingPlane"];
		farClippingPlane = json["farClippingPlane"];

		const std::string skyVal = json["skybox"];
		if (skyVal != skyboxPath)
			skybox = Data::Resources::jsonLoad<Rendering::Skybox>(skyVal);
		skyboxPath = skyVal;
	}
}