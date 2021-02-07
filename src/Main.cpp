/*
	MIT License

	Copyright (c) 2017 Tristan Metz, Leon Brands

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

/*
 TRISTEON CODING STANDARDS
 
 POINTERS
 * Raw pointer: Never owns the referenced object. Do NOT attempt to manage/destroy this object if it is given to you.
 * Unique pointer: Owns the referenced object, will destroy the object automatically.
 * Shared pointer: Never used
 
 NAMING
 * Class/Struct names: UpperCamelCase
 * Functions/Methods: lowerCamelCase
 * Member/Local variables: lowerCamelCase
 * Properties: UpperCamelCase
 * Enum: UpperCamelCase
 * Enum values: EnumName + Underscore upper case: EN_FOO
 * Macro definition: Underscore upper case: UPPER_CASE_MACRO
 * Parameters: UpperCamelCase preceding with p: pUserName

 COMMENTING
 * Every function/method and class, where applicable should be thoroughly commented to explain its purpose, use cases and expected parameter values.
 * Comments on wanted future improvements / changes and/or temporary code should start with TODO.
 
 EDITOR
 * Editor code is allowed to reference engine code
 * The engine should be capable of running without any editor code at all times.
 * Editor code should be encapsulated by #ifdef TRISTEON_EDITOR and #endif
*/

#ifndef TRISTEON_LOGENABLED
#include <windows.h>
#endif

#include "Core/Engine.h"
#include "Core/Message.h"
#include "Core/MessageBus.h"

#ifdef TRISTEON_EDITOR
#include "Editor/TristeonEditor.h"
#endif

using namespace Tristeon;

int main(int argc, char** argv)
{
#ifndef TRISTEON_LOGENABLED //TODO: Implement in-editor debugging tool, disable console alltogether
	FreeConsole();
#endif

	Core::Engine engine{};

#ifdef TRISTEON_EDITOR
	Editor::TristeonEditor editor(&engine);
#else
	//Auto start game with the starting scene loaded in in game/release mode
	Scenes::SceneManager::loadScene(0);
	Core::MessageBus::sendMessage(Core::MessageType::MT_GAME_LOGIC_START);
#endif

	engine.run();
	return 0;
}