#include "Console.h"
#include <iostream>
#include "rlutil.h"

namespace Tristeon
{
	namespace Misc
	{
		void Console::init()
		{
			#if TRISTEON_LOGENABLED
			//Clear and set the color to white
			clear();
			setColor(rlutil::WHITE);
			#endif
		}

		void Console::t_assert(bool condition, std::string errorMessage)
		{
			#if TRISTEON_LOGENABLED
			if (!condition)
				error(errorMessage);
			#endif
		}

		void Console::clear()
		{
			#if TRISTEON_LOGENABLED
			if (system("CLS"))
				system("clear");
			#endif
		}

		void Console::write(std::string data)
		{
			#if TRISTEON_LOGENABLED
			std::cout << data << std::endl;
			#endif
		}

		void Console::warning(std::string data)
		{
			#if TRISTEON_LOGENABLED
			setColor(rlutil::YELLOW);
			std::cout << "[WARNING]\t" << data << std::endl;
			setColor(rlutil::WHITE);
			#endif
		}

		void Console::error(std::string data)
		{
#if TRISTEON_LOGENABLED
			setColor(rlutil::LIGHTRED);
			std::cout << "[ERROR]\t" << data << std::endl;
			setColor(rlutil::WHITE);
#endif

#if (defined(_WIN32) | defined(_WIN64)) && TRISTEON_LOGENABLED
			
			MessageBox(nullptr, data.c_str(), "Oops! Something went wrong...", MB_OK);
#endif
			throw std::runtime_error(data.c_str());
		}
	}
}