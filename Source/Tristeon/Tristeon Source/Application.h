#pragma once
#include "Core/Engine.h"

#ifdef EDITOR
#include "Editor/TristeonEditor.h"
#endif

namespace Tristeon
{
	/**
	 * \brief Application manages and owns the engine and the editor, and allows for communication between the two
	 */
	class Application
	{
	public:
		/**
		 * \brief Cleans up the engine and the editor
		 */
		~Application();

		/**
		 * \brief Creates the engine and the editor and sets up the binding between the two
		 */
		void init();

	private:
		/**
		 * \brief The main engine class
		 */
		Core::Engine* engine = nullptr;

#ifdef EDITOR
		/**
		 * \brief The editor
		 */
		Editor::TristeonEditor* editor = nullptr;
#endif
	};
}
