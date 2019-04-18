#include "Keyboard.h"
#include <GLFW/glfw3.h>
#include <ImGUI/imgui_impl_glfw_vulkan.h>

namespace Tristeon
{
	namespace Misc
	{
		Delegate<KeyCode, KeyMods> Keyboard::onKeyDown;
		Delegate<KeyCode, KeyMods> Keyboard::onKeyUp;
		bool Keyboard::keys[MAXKEYS];
		bool Keyboard::keysUp[MAXKEYS];
		bool Keyboard::keysDown[MAXKEYS];

		bool Keyboard::getKeyDown(KeyCode key)
		{
			return keysDown[key];
		}

		bool Keyboard::getKey(KeyCode key)
		{
			return keys[key];
		}

		bool Keyboard::getKeyUp(KeyCode key)
		{
			return keysUp[key];
		}

		void Keyboard::reset()
		{
			std::fill(std::begin(keysUp), std::end(keysUp), false);
			std::fill(std::begin(keysDown), std::end(keysDown), false);
		}

		void Keyboard::keyCallback(int key, int scancode, int action, int mods)
		{
			if (action == GLFW_PRESS)
			{
				keysDown[key] = true;
				onKeyDown.invoke(static_cast<KeyCode>(key), static_cast<KeyMods>(mods));
			}
			else if (action == GLFW_RELEASE)
			{
				keysUp[key] = true;
				onKeyUp.invoke(static_cast<KeyCode>(key), static_cast<KeyMods>(mods));
			}
			
			keys[key] = action;

#ifdef TRISTEON_EDITOR
			ImGui_ImplGlfwVulkan_KeyCallback(nullptr, key, scancode, action, mods);
#endif
		}
	}
}
