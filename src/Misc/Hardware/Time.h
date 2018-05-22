#pragma once
#include <chrono>

namespace Tristeon
{
	//Forward decl
	namespace Core { namespace Rendering { class Window; } }

	namespace Misc
	{
		/**
		 * \brief The time class is used to get time information from Tristeon
		 */
		class Time
		{
		public:
			//Window can set our private values
			friend Core::Rendering::Window;

			/**
			 * \brief Gets the real time that has passed since the application started
			 * \return Returns the time in seconds
			 */
			static float getTimeSinceStart();
			/**
			 * \brief Gets the current frame deltatime
			 * \return Returns the frame deltatime in seconds
			 */
			static float getDeltaTime();
			/**
			 * \brief Returns the current frames per second
			 * \return The amount of frames per second
			 */
			static float getFPS();

		private:
			/**
			 * \brief Timer to keep track of when the time started
			 */
			static std::chrono::time_point<std::chrono::high_resolution_clock> start;
			/**
			 * \brief The deltatime of the past frame
			 */
			static float deltaTime;
			/**
			 * \brief The amount of frames per second
			 */
			static float fps;
		};
	}
}
