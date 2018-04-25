#include "Manager.h"
#include "Core/ManagerProtocol.h"

void Tristeon::Core::Managers::Manager::init()
{
	//Subscribe to reset
	ManagerProtocol::subscribeToMessage(MT_MANAGER_RESET, [&](Message msg) { reset(); });
}