#pragma once
#include <string>

/**
 * \brief Currently typename is not implemented yet thus currently the class is obsolete
 * but will remain for further planned implementations
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