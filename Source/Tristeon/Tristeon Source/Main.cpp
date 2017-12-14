#include "Application.h"
#include "Math/Vector3.h"

#ifndef LOGDEBUG
#include <windows.h>
#endif

int main(int argc, char* argv)
{
	//Create app
	Tristeon::Application app;

#ifndef LOGDEBUG //TODO: Implement in-editor debugging tool, disable console in editor as well
	FreeConsole(); //Close console if there's no debugging to be done
#endif

	std::cout << "size of vector3: " << sizeof(Tristeon::Core::TObject) << std::endl;

	//Start application
	app.init();
	return 0;
}