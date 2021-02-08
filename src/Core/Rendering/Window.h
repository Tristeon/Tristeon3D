#pragma once
#include <Misc/Property.h>
#include <memory>

struct GLFWwindow;

namespace Tristeon
{
	namespace Core
	{
		class Engine;
		namespace Rendering
		{
			/**
			 * \brief Window manages the creation and destruction of a GLFW window. 
			 */
			class Window
			{
				friend Engine;
				friend std::unique_ptr<Window>;
				friend std::unique_ptr<Window>::deleter_type;
			public:
				/**
				* \brief [READONLY] The width of the window in pixels. 
				*/
				SimpleReadOnlyProperty(Window, width, int);
				
				/**
				* \brief [READONLY] The height of the window in pixels.
				*/
				SimpleReadOnlyProperty(Window, height, int);

				Window();
				~Window();
			private:
				void onResize(int width, int height);
			};
		}
	}
}