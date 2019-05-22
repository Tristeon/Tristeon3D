#pragma once

#if defined(_MSC_VER)
#include "Misc/StringUtils.h"
#include "Misc/Console.h"

/**
 * Wrapper around typeid(T).name(), to enforce a consistent return style across all (supported) compilers.
 */
template<typename T>
std::string getStringTypename()
{
    std::vector<std::string> vec = StringUtils::split(typeid(T).name(), ' ');
    if (vec.size() < 2)
    {
        Tristeon::Misc::Console::warning("Failed to getTypename. Returning empty string!");
        return "";
    }

    std::string result = "";
    for (size_t i = 1; i < vec.size(); i++)
    {
        result += vec[i];
    }
    return result;
}

#define TRISTEON_TYPENAME(T) getStringTypename<T>()

#else
#include <cxxabi.h>

/**
 * Wrapper around typeid(T).name(), to enforce a consistent return style across all (supported) compilers.
 */
template<typename T>
std::string getStringTypename()
{
    int status;
    char* realName = abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);
    std::string result = { realName };
    free(realName);
    return result;
}
#define TRISTEON_TYPENAME(T) getTypename<T>()
#endif()
