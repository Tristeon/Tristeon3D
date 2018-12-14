#pragma once
#include <chrono>

namespace Tristeon
{
	namespace Core 
	{
		class Engine;
		namespace Rendering { class Window; }
	}

	namespace Misc
	{
		/**
		 * The time class is used to get time information from Tristeon
		 */
		class Time
		{
		public:
			friend Core::Rendering::Window;
			friend Core::Engine;

			/**
			 * Gets the real time in seconds that has passed since the application started
			 */
			static float getTimeSinceStart();
			/**
			 * Gets the current frame deltatime in seconds
			 */
			static float getDeltaTime();
			/**
			 * Gets the current amount of frames per second
			 */
			static float getFPS();

		private:
			static std::chrono::time_point<std::chrono::high_resolution_clock> start;
			static float deltaTime;
			static float fps;
		};
	}
}