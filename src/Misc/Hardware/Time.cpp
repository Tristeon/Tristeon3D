#include "Time.h"

namespace Tristeon
{
	namespace Misc
	{
		std::chrono::time_point<std::chrono::high_resolution_clock> Time::start = std::chrono::high_resolution_clock::now();
		float Time::deltaTime = 0;
		float Time::fps = 60;

		float Time::getTimeSinceStart()
		{
			//Get now
			auto const current = std::chrono::high_resolution_clock::now();
			//Time since start is now - start
			//Divide by 1000 to convert from milliseconds to seconds
			return std::chrono::duration_cast<std::chrono::milliseconds>(current - start).count() / 1000.0f;
		}

		float Time::getDeltaTime()
		{
			return deltaTime;
		}

		float Time::getFPS()
		{
			return fps;
		}
	}
}
