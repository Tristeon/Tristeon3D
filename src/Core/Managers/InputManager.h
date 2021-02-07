#pragma once
namespace Tristeon::Core::Managers
{
	/**
	 * InputManager manages all the input calls coming from the window context and caches them for usage as Mouse/Keyboard/Joystick data
	 *
	 * This class is not intended to be accessed or used by users.
	 */
	class InputManager final
	{
	public:
		/**
		 * \param window The GLFW window, used to subscribe to callbacks
		 */
		explicit InputManager();

	private:
		/**
		 * Resets keydown and keyup, called every frame
		 */
		static void resetInput();
	};
}
