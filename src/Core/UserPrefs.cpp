#include "UserPrefs.h"

namespace Tristeon
{
	namespace Core
	{
		std::map<std::string, int> UserPrefs::iUserPrefs;
		std::map<std::string, std::string> UserPrefs::sUserPrefs;
		std::map<std::string, bool> UserPrefs::bUserPrefs;
		std::map<std::string, float> UserPrefs::fUserPrefs;

		int UserPrefs::getIntValue(const std::string& pName)
		{
			return iUserPrefs.find(pName) != iUserPrefs.end() ? iUserPrefs[pName] : -1;
		}

		std::string UserPrefs::getStringValue(const std::string& pName)
		{
			return sUserPrefs.find(pName) != sUserPrefs.end() ? sUserPrefs[pName] : "";
		}

		bool UserPrefs::getBoolValue(const std::string& pName)
		{
			return bUserPrefs.find(pName) != bUserPrefs.end() ? bUserPrefs[pName] : -1;
		}

		float UserPrefs::getFloatValue(const std::string& pName)
		{
			return fUserPrefs.find(pName) != fUserPrefs.end() ? fUserPrefs[pName] : -1;
		}

		bool UserPrefs::hasInt(const std::string& pName)
		{
			return iUserPrefs.find(pName) != iUserPrefs.end();
		}

		bool UserPrefs::hasString(const std::string& pName)
		{
			return sUserPrefs.find(pName) != sUserPrefs.end();
		}

		bool UserPrefs::hasBool(const std::string& pName)
		{
			return bUserPrefs.find(pName) != bUserPrefs.end();
		}

		bool UserPrefs::hasFloat(const std::string& pName)
		{
			return fUserPrefs.find(pName) != fUserPrefs.end();
		}

		void UserPrefs::readPrefs()
		{
			//TODO: Load settings from settings file here
			sUserPrefs["RENDERAPI"] = "VULKAN";
			sUserPrefs["RENDERTECHNIQUE"] = "FORWARD";

			bUserPrefs["FULLSCREEN"] = false;
			iUserPrefs["SCREENWIDTH"] = 1920;
			iUserPrefs["SCREENHEIGHT"] = 980;
		}
	}
}