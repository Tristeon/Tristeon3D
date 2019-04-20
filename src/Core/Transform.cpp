#include "Transform.h"

#include <glm/gtx/matrix_decompose.hpp>
#include "XPlatform/typename.h"

namespace Tristeon
{
	namespace Core
	{
		REGISTER_TYPE_CPP(Transform)

		Transform::~Transform()
		{
			//Remove all our children
			for (int i = 0; i < children.size(); i++)
				children[i]->parent = nullptr;
			children.clear();

			if (parent != nullptr)
				parent->children.remove(this);
			parent = nullptr;
		}

		void Transform::setParent(Transform* parent, bool keepWorldTransform)
		{
			//Can't parent to ourselves. TODO: In a deeper parent hierarchy, we could still accidentally parent loop.
			if (parent == this || parent != nullptr && parent->parent == this)
				return;

			//Deregister ourselves from our old parent
			if (this->parent != nullptr)
				this->parent->children.remove(this);

			//Add ourselves to the new parent
			if (parent != nullptr)
				parent->children.push_back(this);

			if (keepWorldTransform)
			{
				//Store old transformation
				Vector3 const oldGlobalPos = position.get();
				Vector3 const oldGlobalScale = scale.get();
				Quaternion const oldGlobalRot = rotation.get();

				this->parent = parent;

				//Reset transform
				position.set(oldGlobalPos);
				scale.set(oldGlobalScale);
				rotation.set(oldGlobalRot);
			}
			else
			{
				this->parent = parent;
			}
		}

		nlohmann::json Transform::serialize()
		{
			nlohmann::json output;
			output["typeID"] = TRISTEON_TYPENAME(Transform);
			output["instanceID"] = getInstanceID();
			output["parentID"] = parent == nullptr ? "null" : parent->getInstanceID();
			output["localPosition"] = _localPosition.serialize();
			output["localScale"] = _localScale.serialize();
			output["localRotation"] = _localRotation.eulerAngles().serialize();
			return output;
		}

		void Transform::deserialize(nlohmann::json json)
		{
			const std::string instanceIDValue = json["instanceID"];
			instanceID = instanceIDValue;
			const std::string parentIDValue = json["parentID"];
			parentID = parentIDValue;
			_localPosition.deserialize(json["localPosition"]);
			_localScale.deserialize(json["localScale"]);
			Vector3 eulerAngles;
			eulerAngles.deserialize(json["localRotation"]);
			_localRotation = Quaternion::euler(eulerAngles);
		}

		Vector3 Transform::transformPoint(Vector3 point)
		{
			Transform* par = this;
			Quaternion res = rotation;
			while (par->parent != nullptr)
			{
				res = par->parent->rotation.get() * res;
				par = par->parent;
			}

			glm::mat4 m = glm::mat4(res.getGLMQuat());
			glm::vec4 p = m * glm::vec4(point.x, point.y, point.z, 1.0);
			return Vector3(p.x, p.y, p.z);
		}

		Vector3 Transform::inverseTransformPoint(Vector3 point)
		{
			glm::vec4 const res = glm::vec4(point.x, point.y, point.z, 1.0) *  inverse(getTransformationMatrix());
			return{ res.x, res.y, res.z };
		}

		void Transform::lookAt(Transform* target, Vector3 up)
		{
			Vector3 const pos = position.get();
			Vector3 const tar = target->position.get();
			glm::mat4 const transf = inverse(glm::lookAt(glm::vec3(pos.x, pos.y, pos.z ), { tar.x, tar.y, tar.z }, { up.x, up.y, up.z } ));

			glm::vec3 s, p;
			glm::quat r;
			glm::vec3 skew;
			glm::vec4 persp;
			decompose(transf, s, r, p, skew, persp);

			rotation = Quaternion(r);
		}

		Vector3 Transform::up()
		{
			return transformPoint(Vector3::up);
		}

		Vector3 Transform::right()
		{
			return transformPoint(Vector3::right);
		}

		Vector3 Transform::forward()
		{
			return transformPoint(Vector3::forward);
		}

		Vector3 Transform::getGlobalPosition()
		{
			if (parent == nullptr)
				return _localPosition;
			else
				return Vec_Convert3(getTransformationMatrix()[3]);
		}

		void Transform::setGlobalPosition(Vector3 pos)
		{
			if (parent == nullptr)
				_localPosition = pos;
			else
				_localPosition = parent->inverseTransformPoint(pos);
		}

		Vector3 Transform::getGlobalScale()
		{
			if (parent == nullptr)
				return _localScale;

			//Get scale off our transformation matrix
			glm::mat4 const trans = getTransformationMatrix();

			glm::vec3 scale;
			//Unused variables but required in the function
			glm::quat rotation;
			glm::vec3 translation;
			glm::vec3 skew;
			glm::vec4 perspective;
			decompose(trans, scale, rotation, translation, skew, perspective);

			return Vec_Convert3(scale);
		}

		void Transform::setGlobalScale(Vector3 scale)
		{
			if (parent == nullptr)
				_localScale = scale;
			else
			{
				glm::mat4 const p = parent->getTransformationMatrix();

				glm::vec3 s;
				glm::quat r;
				glm::vec3 t;
				glm::vec3 sk;
				glm::vec4 per;
				decompose(p, s, r, t, sk, per);

				//New localscale, defined as x
				//x * parent = goal //When we multiply our new localscale with parent it should equal to our goal
				//x = goal / parent //Which means that we can define x as this
				//so
				//_localScale = scale / parent
				_localScale = scale / Vector3(s.x, s.y, s.z);
			}
		}

		Quaternion Transform::getGlobalRotation()
		{
			if (parent == nullptr)
				return _localRotation;

			//Get global rotation off our matrix
			glm::mat4 const trans = getTransformationMatrix();
			glm::quat rotation;

			//Unused variables, required for the function
			glm::vec3 scale;
			glm::vec3 translation;
			glm::vec3 skew;
			glm::vec4 perspective;
			decompose(trans, scale, rotation, translation, skew, perspective);

			return Quaternion(rotation);
		}

		void Transform::setGlobalRotation(Quaternion rot)
		{
			if (parent == nullptr)
				_localRotation = rot;
			else
			{
				//Throwaway values
				glm::vec3 scale;
				glm::vec3 translation;
				glm::vec3 skew;
				glm::vec4 perspective;

				//Get parent info
				glm::mat4 const p = parent->getTransformationMatrix();
				glm::quat rotation;
				decompose(p, scale, rotation, translation, skew, perspective);

				glm::mat4 const pr = glm::mat4(rotation);			//Parent rotation
				glm::mat4 const gl = glm::mat4(rot.getGLMQuat());	//Goal rotation

				//Local needs to be x so that
				//pr * x = gl
				//So x = gl / pr
				glm::mat4 const x = gl / pr;
				decompose(x, scale, rotation, translation, skew, perspective);

				//Local rotation
				glm::quat const local = rotation;
				_localRotation = Quaternion(local);
			}
		}

		glm::mat4 Transform::getTransformationMatrix()
		{
			//Get parent transformation (recursive)
			glm::mat4 p = glm::mat4(1.0f);
			if (parent != nullptr)
				p *= parent->getTransformationMatrix();

			//Get transformation
			glm::mat4 const t = glm::translate(glm::mat4(1.0f), Vec_Convert3(localPosition.get()));
			glm::mat4 const r = glm::mat4(localRotation.get().getGLMQuat());
			glm::mat4 const s = glm::scale(glm::mat4(1.0f), Vec_Convert3(localScale.get()));

			//Apply and return
			return t * r * s * p;
		}

		void Transform::rotate(Vector3 axis, float rot)
		{
			rotation = rotation.get().rotate(axis, rot);
		}

		void Transform::translate(Vector3 t)
		{
			_localPosition += transformPoint(t);
		}

		void Transform::translate(float x, float y, float z)
		{
			translate(Vector3(x, y, z));
		}

		Transform* Transform::getParent() const
		{
			return parent;
		}
	}
}