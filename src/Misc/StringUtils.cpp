#include "StringUtils.h"
#include <iterator>
#include <vector>
#include "Console.h"

std::vector<std::string> StringUtils::split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

std::string StringUtils::generateRandom(int length)
{
	if (length <= 0)
		throw std::invalid_argument("The length parameter of StringUtils::generateRandom must be > 0!");

	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	static const int size = sizeof(alphanum);
	
	std::string output;

	for (int i = 0; i < length; ++i) {
		output.push_back(alphanum[rand() % (size - 1)]);
	}

	return output;
}
