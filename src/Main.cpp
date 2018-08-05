#include "Application.h"

#ifndef TRISTEON_LOGENABLED
#include <windows.h>
#endif

int main(int argc, char** argv)
{
#ifndef TRISTEON_LOGENABLED //TODO: Implement in-editor debugging tool, disable console in editor as well
	FreeConsole(); //Close console if there's no debugging to be done
#endif

    Tristeon::Application app;
	return 0;
}
