#include "Window.h"

#include "Core/Settings.h"
#include <Core/ManagerProtocol.h>
#include "Core/Message.h"

#include "Math/Vector2.h"

#include "Misc/Console.h"
#include "Misc/Hardware/Time.h"

#include <GLFW/glfw3.h>

//Shortcut for calling a static function with the userpointer provided by GLFW
#define USER_POINTER_CALL(window) reinterpret_cast<Window*>(glfwGetWindowUserPointer(window))

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			Window::~Window()
			{
				ManagerProtocol::sendMessage(MT_QUIT);
				//Destroy glfw
				glfwDestroyWindow(window);
				glfwTerminate();
			}

			void Window::init()
			{
				//Already have a window? don't create another one
				if (_window != nullptr)
					return;

				//Set error callback before anything else, so all problems will be recorded immediately
				glfwSetErrorCallback([](int error, const char* description) { errorCallback(error, description); });

				//Initialize GLFw
				glfwInit();

				//Set our GLFW preferences
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
				glfwWindowHint(GLFW_RESIZABLE, true);

				//Get requested window size
				int width, height;
				GLFWmonitor* monitor = nullptr;
				if (Settings::getFullScreen())
				{
					//Get fullscreen size
					monitor = glfwGetPrimaryMonitor();
					glfwGetMonitorPhysicalSize(monitor, &width, &height);
				}
				else
				{
					width = Settings::getScreenWidth();
					height = Settings::getScreenHeight();
				}
				//Store window size
				w = width;
				h = height;

				//Callback for inherited classes
				onPreWindowCreation();

				//Create window
				_window = glfwCreateWindow(width, height, "Tristeon", monitor, nullptr);
				if (window == nullptr)
					Misc::Console::error("Failed to open GLFW window!");
				//Store this in the GLFW userdata for reference later
				glfwSetWindowUserPointer(window, this);

				//Set GLFW callbacks
				setupCallbacks();

				//Callback for inherited classes
				onPostWindowCreation();
			}

			void Window::mainLoop()
			{
				//Block if we're already running
				if (running)
				{
					Misc::Console::warning("Trying to call Window::mainLoop() while the main loop is already running!");
					return;
				}
				running = true;

				float lastTime = glfwGetTime();

				//FixedUpdate
				float fixedUpdateTime = 0;

				//FPS
				int elapsedFrames = 0;
				int elapsedTime = 0;

				while (!glfwWindowShouldClose(window))
				{
					glfwPollEvents();

					//Measure time
					float const newTime = glfwGetTime();
					Misc::Time::deltaTime = newTime - lastTime;
					lastTime = newTime;

					//Calculate the frames per second
					elapsedFrames++;
					elapsedTime += Misc::Time::deltaTime;
					if (elapsedTime >= 1)
					{
						Misc::Time::fps = float(elapsedFrames);
						elapsedFrames = 0;
						elapsedTime--;
					}

					//If gameplay is active, call gameplay
					if (inPlayMode)
					{
						fixedUpdateTime += Misc::Time::deltaTime;
						//Call fixedUpdate if needed until we caught up
						while (fixedUpdateTime > 1.0f / 50.0f) //TODO: Make fixed delta time variable
						{
							ManagerProtocol::sendMessage(MT_FIXEDUPDATE);
							fixedUpdateTime -= 1.0f / 50.0f;
						}

						//Update functions
						ManagerProtocol::sendMessage(MT_UPDATE);
						ManagerProtocol::sendMessage(MT_LATEUPDATE);
					}

					//Render unless if we're minimized
					if (w != 0 && h != 0)
					{
						ManagerProtocol::sendMessage(MT_PRERENDER);
						ManagerProtocol::sendMessage(MT_RENDER);
						ManagerProtocol::sendMessage(MT_POSTRENDER);

						ManagerProtocol::sendMessage(MT_AFTERFRAME);
					}
				}
			}

			void Window::onPreWindowCreation()
			{
				//Empty
			}

			void Window::onPostWindowCreation()
			{
				//Empty
			}

			void Window::setupCallbacks()
			{
				//GLFW callbacks
				glfwSetWindowSizeCallback(window, [](GLFWwindow* w, int width, int height) { USER_POINTER_CALL(w)->onResize(width, height); });
				glfwSetWindowFocusCallback(window, [](GLFWwindow* w, int focus) { USER_POINTER_CALL(w)->onFocus.invoke(focus == true); });

				//Game logic start/stop calls
				ManagerProtocol::subscribeToMessage(MT_GAME_LOGIC_START, [&](Message msg)
				{
					ManagerProtocol::sendMessage(MT_START);
					inPlayMode = true;
				});
				ManagerProtocol::subscribeToMessage(MT_GAME_LOGIC_STOP, [&](Message msg) { inPlayMode = false; });
			}

			void Window::onResize(int width, int height)
			{
				//Store our new size
				w = width;
				h = height;
				
				//Share our size information with the world
				Math::Vector2 size = Math::Vector2(width, height);
				ManagerProtocol::sendMessage(Message(MT_WINDOW_RESIZE, &size));
			}

			void Window::errorCallback(int error, const char* description)
			{
				Misc::Console::error("GLFW Error. Error code: " + std::to_string(error) + ". Description: " + description);
			}
		}
	}
}
