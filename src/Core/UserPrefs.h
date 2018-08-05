#pragma once
#include <map>
#include <string>

namespace Tristeon
{
	namespace Core
	{
		/**
		 * \brief Static class that loads and exposes user settings
		 */
		class UserPrefs
		{
		public:
			/**
			 * \brief Loads in user preferences
			 */
			static void readPrefs();

			static int getIntValue(const std::string& pName) { return iUserPrefs.find(pName) != iUserPrefs.end() ? iUserPrefs[pName] : -1; }
			static std::string getStringValue(const std::string& pName) { return sUserPrefs.find(pName) != sUserPrefs.end() ? sUserPrefs[pName] : ""; }
			static bool getBoolValue(const std::string& pName) { return bUserPrefs.find(pName) != bUserPrefs.end() ? bUserPrefs[pName] : -1; }
			static float getFloatValue(const std::string& pName) { return fUserPrefs.find(pName) != fUserPrefs.end() ? fUserPrefs[pName] : -1; }
		private:
			static std::map<std::string, int> iUserPrefs;
			static std::map<std::string, std::string> sUserPrefs;
			static std::map<std::string, bool> bUserPrefs;
			static std::map<std::string, float> fUserPrefs;
		};
	}
}
