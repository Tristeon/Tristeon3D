#pragma once
#include <string>
#include <typeinfo> //Required by some compilers
#include <Misc/Console.h>
#include <Misc/StringUtils.h>

namespace Tristeon::Core
{
	/**
	 * A cross-compiler/platform class for generating consistent type data.
	 */
	template <typename T>
	struct Type
	{
		/**
		 * Get the type's full name, including its namespace.
		 */
		static std::string fullName();
	};


#ifdef _MSC_VER
	template <typename T>
	std::string Type<T>::fullName()
	{
		//MSVC generates "typename <Class>". Remove the typename bit.
		const std::string name = typeid(T).name();
		std::vector<std::string> vec = StringUtils::split(name, ' ');
		if (vec.size() < 2)
		{
			Misc::Console::warning("Type failed to interpret the MSVC typename. Returning direct typeid(T).name(): " + std::string(typeid(T).name()));
			return name;
		}

		std::string result;
		for (auto i = 1; i < vec.size(); i++)
		{
			result += vec[i];
		}
		return result;
	}
#else
#include <cxxabi.h>

	template <typename T>
	String Type<T>::fullName()
	{
		//Demangle the typename generated by GCC
		int status;
		char* realName = abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);
		std::string result = { realName };
		free(realName);
		return result;
	}
#endif
}