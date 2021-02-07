#pragma once
#include "Core/Rendering/Window.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			class WindowContext
			{
			public:
				WindowContext();
				virtual void prepareFrame() = 0;
				virtual void finishFrame() = 0;
				virtual void resize(int width, int height) = 0;
			};
		}
	}
}
