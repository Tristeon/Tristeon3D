﻿#include "WindowContext.h"
#include "Core/ManagerProtocol.h"
#include "Math/Vector2.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			WindowContext::WindowContext(Window* pWindow) : window(pWindow)
			{
				//ManagerProtocol::subscribeToMessage(MT_WINDOW_RESIZE, [&](Message msg)
				//{
				//	Math::Vector2* size = dynamic_cast<Math::Vector2*>(msg.userData);
				//	resize(static_cast<int>(size->x), static_cast<int>(size->y));
				//});
			}
		}
	}
}
