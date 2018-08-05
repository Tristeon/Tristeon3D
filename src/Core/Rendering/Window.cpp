#include "Window.h"

#include "Core/UserPrefs.h"
#include <Core/MessageBus.h>
#include "Core/Message.h"

#include "Math/Vector2.h"

#include "Misc/Console.h"
#include "Misc/Hardware/Time.h"

#include <GLFW/glfw3.h>

//Shortcut for getting window from the GLFW user pointer
#define USER_POINTER_CALL(window) reinterpret_cast<Window*>(glfwGetWindowUserPointer(window))

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			Window::~Window()
			{
				MessageBus::sendMessage(MT_QUIT);
				glfwDestroyWindow(_window);
				glfwTerminate();
			}

			void Window::init()
			{
				//Set error callback before anything else, so all problems will be recorded immediately
				glfwSetErrorCallback([](int error, const char* description) { errorCallback(error, description); });

				//Initialize GLFw
				glfwInit();

				//Set our GLFW preferences
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
				glfwWindowHint(GLFW_RESIZABLE, true);

				//Get requested window size
				GLFWmonitor* monitor = nullptr;
				if (UserPrefs::getBoolValue("FULLSCREEN"))
				{
					monitor = glfwGetPrimaryMonitor();
					int width, height;
					glfwGetMonitorPhysicalSize(monitor, &width, &height);
					w = width;
					h = height;
				}
				else
				{
					w = UserPrefs::getIntValue("SCREENWIDTH");
					h = UserPrefs::getIntValue("SCREENHEIGHT");
				}

				onPreWindowCreation();

				_window = glfwCreateWindow(width, height, "Tristeon", monitor, nullptr);
				if (window.get() == nullptr)
					Misc::Console::error("Failed to open GLFW window!");
				glfwMaximizeWindow(_window);
				glfwSetWindowUserPointer(window.get(), this);

				//Set GLFW callbacks
				setupCallbacks();
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

				double lastTime = glfwGetTime();
				float fixedUpdateTime = 0;
				int frames = 0;
				float time = 0;

				while (!glfwWindowShouldClose(window.get()))
				{
					glfwPollEvents();

					//Measure time
					float const newTime = glfwGetTime();
					Misc::Time::deltaTime = newTime - lastTime;
					lastTime = newTime;

					//Calculate the frames per second
					frames++;
					time += Misc::Time::deltaTime;
					if (time >= 1)
					{
						Misc::Time::fps = float(frames);
						frames = 0;
						time--;
					}

					//If gameplay is active, call gameplay
					if (inPlayMode)
					{
						fixedUpdateTime += Misc::Time::deltaTime;
						//Call fixedUpdate if needed until we caught up
						while (fixedUpdateTime > 1.0f / 50.0f) //TODO: Make fixed delta time variable
						{
							MessageBus::sendMessage(MT_FIXEDUPDATE);
							fixedUpdateTime -= 1.0f / 50.0f;
						}

						//Update functions
						MessageBus::sendMessage(MT_UPDATE);
						MessageBus::sendMessage(MT_LATEUPDATE);
					}

					//Render unless if we're minimized
					if (w != 0 && h != 0)
					{
						MessageBus::sendMessage(MT_PRERENDER);
						MessageBus::sendMessage(MT_RENDER);
						MessageBus::sendMessage(MT_POSTRENDER);

						MessageBus::sendMessage(MT_AFTERFRAME);
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
				glfwSetWindowFocusCallback(window, [](GLFWwindow* w, int focus) { USER_POINTER_CALL(w)->onFocus.invoke(focus == 1); });

				//Game logic start/stop calls
				MessageBus::subscribeToMessage(MT_GAME_LOGIC_START, [&](Message msg)
				{
					MessageBus::sendMessage(MT_START);
					inPlayMode = true;
				});
				MessageBus::subscribeToMessage(MT_GAME_LOGIC_STOP, [&](Message msg) { inPlayMode = false; });
			}

			void Window::onResize(int width, int height)
			{
				//Store our new size
				w = width;
				h = height;
				
				//Share our size information with the world
				Math::Vector2 size = Math::Vector2((float)width, (float)height);
				MessageBus::sendMessage(Message(MT_WINDOW_RESIZE, &size));
			}

			void Window::errorCallback(int error, const char* description)
			{
				Misc::Console::error("GLFW Error. Error code: " + std::to_string(error) + ". Description: " + description);
			}
		}
	}
}
