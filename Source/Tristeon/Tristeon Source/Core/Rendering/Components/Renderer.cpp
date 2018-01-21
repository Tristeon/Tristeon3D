#include "Renderer.h"
#include "Core/Message.h"
#include "Core/ManagerProtocol.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			void Renderer::render()
			{
				//Call rendering API specific render call
				if (renderer != nullptr)
					renderer->render();
			}

			Renderer::~Renderer()
			{
				//Deregister ourselves
				if (registered)
					ManagerProtocol::sendMessage({ MT_RENDERINGCOMPONENT_DEREGISTER, dynamic_cast<TObject*>(this) });

				//Cleanup renderer
				if (renderer != nullptr)
					delete renderer;
			}

			void Renderer::init()
			{
				if (!registered)
					ManagerProtocol::sendMessage({MT_RENDERINGCOMPONENT_REGISTER, dynamic_cast<TObject*>(this) });
				registered = true;
			}
		}
	}
}
