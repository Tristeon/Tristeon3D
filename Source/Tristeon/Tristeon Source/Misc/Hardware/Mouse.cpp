#include "Mouse.h"
#include <glfw/glfw3.h>
#include "Keyboard.h"
#include "../../../ImGUI/imgui_impl_glfw_vulkan.h"

namespace Tristeon
{
	namespace Misc
	{
		//Static decl
		Delegate<MouseButton, KeyMods> Mouse::onButtonDown;
		Delegate<MouseButton, KeyMods> Mouse::onButtonUp;
		
		Delegate<> Mouse::onEnteredWindow;
		Delegate<> Mouse::onExitedWindow;

		bool Mouse::buttons[MAXBUTTONS];
		bool Mouse::buttonsUp[MAXBUTTONS];
		bool Mouse::buttonsDown[MAXBUTTONS];
		bool Mouse::inWindow = false;

		Math::Vector2 Mouse::position;
		Math::Vector2 Mouse::deltaPos;
		Math::Vector2 Mouse::scroll;

		bool Mouse::getButtonDown(MouseButton button)
		{
			return buttonsDown[button];
		}

		bool Mouse::getButton(MouseButton button)
		{
			return buttons[button];
		}

		bool Mouse::getButtonUp(MouseButton button)
		{
			return buttonsUp[button];
		}

		bool Mouse::getInWindow()
		{
			return inWindow;
		}

		Math::Vector2 Mouse::getPosition()
		{
			return position;
		}

		Math::Vector2 Mouse::getMouseDelta()
		{
			return deltaPos;
		}

		Math::Vector2 Mouse::getScroll()
		{
			return scroll;
		}

		void Mouse::reset()
		{
			//Clear data
			std::fill(std::begin(buttonsDown), std::end(buttonsDown), false);
			std::fill(std::begin(buttonsUp), std::end(buttonsUp), false);
			deltaPos = Math::Vector2::zero();
			scroll = Math::Vector2::zero();
		}

		void Mouse::buttonCallback(int button, int action, int mods)
		{
			//Button was pressed
			if (action == GLFW_PRESS)
			{
				buttonsDown[button] = true;
				//User callback for custom actions / buttons with modifiers
				onButtonDown.invoke(static_cast<MouseButton>(button), static_cast<KeyMods>(mods));
			}

			//Buttons was released
			if (action == GLFW_RELEASE)
			{
				buttonsUp[button] = true;
				onButtonUp.invoke(static_cast<MouseButton>(button), static_cast<KeyMods>(mods));
			}

			//Store button state
			buttons[button] = action;

#ifdef EDITOR
			//Imgui callback
			ImGui_ImplGlfwVulkan_MouseButtonCallback(nullptr, button, action, mods);
#endif
		}

		void Mouse::positionCallback(int x, int y)
		{
			//Calculate movement and store new position
			deltaPos = Math::Vector2(x - position.x, y - position.y);
			position = Math::Vector2(x, y);
		}

		void Mouse::windowEnterCallback(bool enter)
		{
			//Store new info and call callbacks
			inWindow = enter;

			if (enter)
				onEnteredWindow.invoke();
			else
				onExitedWindow.invoke();
		}

		void Mouse::scrollCallback(double x, double y)
		{
			//Store new info and call imgui callback
			scroll = Math::Vector2(x, y);
#ifdef EDITOR
			//Imgui callback
			ImGui_ImplGlfwVulkan_ScrollCallback(nullptr, x, y);
#endif
		}
	}
}