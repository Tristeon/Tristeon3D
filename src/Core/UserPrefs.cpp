#include "UserPrefs.h"

namespace Tristeon
{
	namespace Core
	{
		std::map<std::string, int> UserPrefs::iUserPrefs;
		std::map<std::string, std::string> UserPrefs::sUserPrefs;
		std::map<std::string, bool> UserPrefs::bUserPrefs;
		std::map<std::string, float> UserPrefs::fUserPrefs;

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