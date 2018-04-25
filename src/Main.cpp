#include "Application.h"

#if !TRISTEON_LOGENABLED
#include <windows.h>
#endif

int main(int argc, char* argv)
{
	//Create app
	Tristeon::Application app;

#if !TRISTEON_LOGENABLED //TODO: Implement in-editor debugging tool, disable console in editor as well
	FreeConsole(); //Close console if there's no debugging to be done
#endif

	//Start application
	app.init();
	return 0;
}