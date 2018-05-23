#pragma once

#if defined(_MSC_VER)
#include "Misc/StringUtils.h"
#include "Misc/Console.h"
template<typename T>
std::string getTypename()
{
    std::string name = typeid(T).name();
    std::vector<std::string> vec = StringUtils::split(name, ' ');
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

#define TRISTEON_TYPENAME(T) getTypename<T>()

#else
#include <cxxabi.h>
template<typename T>
std::string getTypename()
{
    int status;
    char* realName = abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);
    std::string result = { realName };
    free(realName);
    return result;
}
#define TRISTEON_TYPENAME(T) getTypename<T>()
#endif()
