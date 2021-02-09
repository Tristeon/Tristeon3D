#pragma once
#include "json.hpp"
#include <Core/Type.h>

namespace Tristeon::Core
{
	class Serializable
	{
	public:
		virtual ~Serializable() = default;

		/**
		 * \brief Serialize interface that is called on derived instance of serializable to obtain json data of objects
		 */
		virtual nlohmann::json serialize() { return nlohmann::json(); }
		/**
		 * \brief Deserialize interface for classes to decide how to use json data to load in data into their class
		 */
		virtual void deserialize(nlohmann::json json) {}

		/**
		 * \brief Checks if the T is the exact same type as this one. (Does not work with inheritance yet)
		 */
		template <typename T> bool isType();
	};

	template<typename T>
	inline bool Serializable::isType()
	{
		return serialize()["typeID"] == Tristeon::Core::Type<T>::fullName();
	}
}