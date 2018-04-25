#include "Settings.h"
#include "Rendering/RenderAPI.h"
#include "Rendering/RenderTechniques/RTechniques.h"

namespace Tristeon
{
	namespace Core
	{
		//Static
		Rendering::RenderAPI Settings::renderAPI;
		Rendering::RTechniques Settings::renderTechnique;

		int Settings::screenWidth;
		int Settings::screenHeight;
		bool Settings::fullscreen;

		void Settings::init()
		{
			//TODO: Load settings from settings file here
			renderAPI = Rendering::RAPI_Vulkan;
			renderTechnique = Rendering::RT_Forward;

			fullscreen = false;
			screenWidth = 1920;
			screenHeight = 980;
		}

		Rendering::RenderAPI Settings::getRenderAPI()
		{
			return renderAPI;
		}

		Rendering::RTechniques Settings::getRenderTechnique()
		{
			return renderTechnique;
		}

		int Settings::getScreenWidth()
		{
			return screenWidth;
		}

		int Settings::getScreenHeight()
		{
			return screenHeight;
		}

		bool Settings::getFullScreen()
		{
			return fullscreen;
		}
	}
}