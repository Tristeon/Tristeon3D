#pragma once
#include "Rendering/RenderAPI.h"
#include "Rendering/RenderTechniques/RTechniques.h"

namespace Tristeon
{
	namespace Core
	{
		/**
		 * \brief Settings is a class that loads in user/project settings.
		 */
		class Settings
		{
		public:
			/**
			 * \brief Load the data
			 */
			static void init();

			/**
			 * \return Returns the user selected rendering API
			 */
			static Rendering::RenderAPI getRenderAPI();
			/**
			 * \return Returns the user selected rendering Technique 
			 */
			static Rendering::RTechniques getRenderTechnique();

			/**
			 * \return Retuns the user selected screen width
			 */
			static int getScreenWidth();
			/**
			 * \return Returns the user selected screen height 
			 */
			static int getScreenHeight();
			/**
			 * \return Returns true if the user wants fullscreen
			 */
			static bool getFullScreen();
		private:
			/**
			 * \brief The user selected rendering API
			 */
			static Rendering::RenderAPI renderAPI;
			/**
			 * \brief The user selected rendering technique
			 */
			static Rendering::RTechniques renderTechnique;

			/**
			 * \brief The user selected screen width
			 */
			static int screenWidth;
			/**
			 * \brief The user selected screen height
			 */
			static int screenHeight;
			/**
			 * \brief True if the user wants fullscreen
			 */
			static bool fullscreen;
		};
	}
}
