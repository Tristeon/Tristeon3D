#pragma once
#include <string>

namespace Tristeon::Misc
{
	/**
	 * Console defines basic printing/asserting behavior that will be logged to the editor or to a console
	 */
	class Console final
	{
	public:
		/**
		 * Sets the standard console color
		 */
		static void init();

		/**
		 * Throws an error if the given condition is not true
		 * \param condition The condition you expect to be true
		 * \param errorMessage The error message to be shown in the error pop-up
		 *
		 * \exception runtime_error If condition is false
		 */
		static void t_assert(bool condition, std::string errorMessage);
		/**
		 * Clears the console
		 */
		static void clear();

		/**
		 * Writes the given data to the console
		 */
		static void write(std::string data);

		/**
		 * Writes a warning with the given data to the editor/console
		 */
		static void warning(std::string data);
		/**
		 * Opens an error pop-up window and stops the program.
		 *
		 * \exception runtime_error
		 */
		static void error(std::string data);
	};
}
