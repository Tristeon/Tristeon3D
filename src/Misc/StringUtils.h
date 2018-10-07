#pragma once
#include <sstream>
#include <vector>

/**
 * StringUtils is a utility class that implements tedious / repetitive string based behavior that isn't provided by the standard library.
 */
class StringUtils final
{
public:
	/**
	 * Splits a string into separate strings based on the given delimiter.
	 */
	static std::vector<std::string> split(const std::string& s, char delim);

	/**
	 * Generates a string with random characters with a given length.
	 */
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
