#include "Resources.h"

namespace Tristeon::Data
{
	std::map<std::string, std::unique_ptr<Core::TObject>> Resources::_persistentResources{};
	std::map<std::string, std::unique_ptr<Core::TObject>> Resources::_sceneResources{};
}