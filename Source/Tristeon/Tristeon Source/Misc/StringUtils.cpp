#include "StringUtils.h"
#include <iterator>
#include <vector>

std::vector<std::string> StringUtils::split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}