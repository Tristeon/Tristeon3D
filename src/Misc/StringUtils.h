#pragma once
#include <sstream>
#include <vector>

class StringUtils
{
public:
	static std::vector<std::string> split(const std::string& s, char delim);

	static std::string generateRandom(int length);

private:
	template <class Out>
	static void split(const std::string& s, char delim, Out result);
};

template<typename Out>
void StringUtils::split(const std::string &s, char delim, Out result) {
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (getline(ss, item, delim)) {
		*(result++) = item;
	}
}
