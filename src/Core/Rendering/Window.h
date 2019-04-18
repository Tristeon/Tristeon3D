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
			 * Window provides a few virtual functions to allow rendering API specific behavior to be defined in inherited window classes.
			 * Window provides a basic API to read and control window functionality, like changing size, position, window mode, etc.
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
				/**
				 * \brief [READONLY] The GLFWWindow. Can be used in a variety of glfw calls. See the GLFW API: http://www.glfw.org/docs/latest/
				 */
				SimpleReadOnlyProperty(Window, window, GLFWwindow*);

			protected:
				/**
				 * Allows for additional features to be added before the GLFW window is created. 
				 * GLFWWindow is still null at this point in time!
				 */
				virtual void onPreWindowCreation() {}

				/**
				 * Allows for additional features to be added after the GLFW window is created.
				 * GLFWWindow is fully initialized and ready to be used before this call.
				 */
				virtual void onPostWindowCreation() {}

				virtual ~Window();
				Window() = default;
			private:
				/**
				* The Init function creates the window and subscribes to window callback functions.
				* This function is intended to be used as if it were a constructor. It is separate because it allows us to call virtual functions inside.
				*/
				void init();
				void onResize(int width, int height);
			};
		}
	}
}
