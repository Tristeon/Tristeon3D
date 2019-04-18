#pragma once
#include <map>
#include <string>

namespace Tristeon
{
	namespace Core
	{
		/**
		 * Static class that loads and exposes user settings.
		 */
		class UserPrefs final
		{
			friend class Engine;
		public:
			/**
			 * Gets the int value that's connected to pName. Defaults to -1 if it said name can't be found.
			 */
			static int getIntValue(const std::string& pName);
			/**
			 * Gets the string value that's connected to pName. Defaults to "" if said name can't be found.
			 */
			static std::string getStringValue(const std::string& pName);
			/**
			 * Gets the boolean value that's connected to pName. Defaults to -1 if said name can't be found.
			 */
			static bool getBoolValue(const std::string& pName);
			/**
			 * Gets the float value that's connected to pName. Defaults to -1 if said name can't be found.
			 */
			static float getFloatValue(const std::string& pName);

			/**
			 * Returns true if an integer with the given name is defined.
			 */
			static bool hasInt(const std::string& pName);
			/**
			 * Returns true if a string with the given name is defined.
			 */
			static bool hasString(const std::string& pName);
			/**
			 * Returns true if a bool with the given name is defined.
			 */
			static bool hasBool(const std::string& pName);
			/**
			 * Returns true if a float with the given name is defined.
			 */
			static bool hasFloat(const std::string& pName);
		private:
			static void readPrefs();
			static std::map<std::string, int> iUserPrefs;
			static std::map<std::string, std::string> sUserPrefs;
			static std::map<std::string, bool> bUserPrefs;
			static std::map<std::string, float> fUserPrefs;

			UserPrefs() = delete;
			~UserPrefs() = delete;
		};
	}
}