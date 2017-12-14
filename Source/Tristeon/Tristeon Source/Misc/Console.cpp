#include "Console.h"
#include <iostream>
#include "rlutil.h"

namespace Tristeon
{
	namespace Misc
	{
		void Console::init()
		{
			#if defined(LOGDEBUG)
			//Clear and set the color to white
			clear();
			setColor(rlutil::WHITE);
			#endif
		}

		void Console::t_assert(bool condition, std::string errorMessage)
		{
			#if defined(LOGDEBUG)
			if (!condition)
				error(errorMessage);
			#endif
		}

		void Console::clear()
		{
			#if defined(LOGDEBUG)
			if (system("CLS"))
				system("clear");
			#endif
		}

		void Console::write(std::string data)
		{
			#if defined(LOGDEBUG)
			std::cout << data << std::endl;
			#endif
		}

		void Console::warning(std::string data)
		{
			#if defined(LOGDEBUG)
			setColor(rlutil::YELLOW);
			std::cout << "[WARNING]\t" << data << std::endl;
			setColor(rlutil::WHITE);
			#endif
		}

		void Console::error(std::string data)
		{
#ifdef LOGDEBUG
			setColor(rlutil::LIGHTRED);
			std::cout << "[ERROR]\t" << data << std::endl;
			setColor(rlutil::WHITE);
#endif

#if (defined(_WIN32) | defined(_WIN64)) && defined(LOGDEBUG)
			if (MessageBox(nullptr, data.c_str(), "Oops! Something went wrong... Break?", MB_YESNO) == IDYES)
				throw std::runtime_error(data.c_str());
#endif
		}
	}
}