#pragma once
#include <Misc/Property.h>
#include "Misc/Delegate.h"

//Forward decl
struct GLFWwindow;

namespace Tristeon
{
	namespace Core
	{
		//Forward decl
		class ManagerProtocol;

		namespace Rendering
		{
			/**
			 * \brief Window manages the creation and destruction of a GLFW window, the main engine loop and sends out callbacks during the loop
			 */
			class Window
			{
			public:
				/**
				 * \brief Destroys the glfw window and cleans up all other resources
				 */
				virtual ~Window();

				/**
				 * \brief Creates the GLFW window and sets up the callbacks
				 */
				virtual void init();
				/**
				 * \brief Starts the main engine loop. This function won't return until the end of the application
				 */
				virtual void mainLoop();

				/**
				 * \brief Callback function that gets called before the GLFW window has been created.
				 * Can be overriden by inherited classes to describe API specific behavior
				 */
				virtual void onPreWindowCreation();
				/**
				 * \brief Callback function that gets called after the GLFW window has been created.
				 * Can be overriden by inherited classes to describe API specific behavior
				 */
				virtual void onPostWindowCreation();

				/**
				* \brief Read only. Returns the width of the window
				*/
				ReadOnlyProperty(int, width);
				GetPropertyConst(width) { return w; }
				
				/**
				* \brief Read only. Returns the height of the window
				*/
				ReadOnlyProperty(int, height);
				GetPropertyConst(height) { return h; }

				/**
				 * \brief Callback function for when the window gains/loses focus
				 */
				Misc::Delegate<bool> onFocus;

				/**
				 * \brief The GLFW window
				 */
				ReadOnlyProperty(GLFWwindow*, window);
				GetPropertyConst(window) { return _window; }
			private:
				/**
				 * \brief GLFW error callback, calls Console::error()
				 * \param error The error id
				 * \param description The description of the error
				 */
				static void errorCallback(int error, const char* description);
				
				/**
				 * \brief Subscribes to all the callbacks
				 */
				void setupCallbacks();
				/**
				 * \brief Callback function for when the window resizes.
				 * \param width The new width of the window
				 * \param height The new height of the window
				 */
				void onResize(int width, int height);

				/**
				 * \brief Describes if the window is in playmode or not
				 */
				bool inPlayMode = false;

				/**
				 * \brief The width of the window
				 */
				int w = 0;
				/**
				 * \brief The height of the window
				 */
				int h = 0;
				/**
				 * \brief The GLFW window
				 */
				GLFWwindow* _window = nullptr;

				/**
				 * \brief Keeps track of if we're already running or not, to make sure that we'll never start the gameloop twice
				 */
				bool running = false;
			};
		}
	}
}
