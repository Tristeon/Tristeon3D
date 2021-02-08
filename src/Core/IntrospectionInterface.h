#pragma once
#include <string>

/**
 * \brief Currently the introspection interface is not implemented yet, but will remain for further planned implementations
 */
class IntrospectionInterface
{
	friend struct TypeRegister;
public:
	std::string getTypeName() { return typeName; }

private:
	void setTypeName(std::string typeName)
	{
		this->typeName = typeName;
	}
	std::string typeName;
};