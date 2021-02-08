#include "Resources.h"

namespace Tristeon
{
	std::map<std::string, std::unique_ptr<Core::TObject>> Resources::_loadedResources{};
}