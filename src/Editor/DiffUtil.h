#pragma once
#ifdef TRISTEON_EDITOR

#include <memory>
#include <map>

namespace Tristeon {
	namespace Core {
		class GameObject;
	}
}


namespace Tristeon
{
	namespace Editor
	{
		/**
		* \brief Conceptual class without implementation, will be subject to change
		 */
		class DiffUtil
		{
		public:
			void takeSnapshot(std::shared_ptr<Core::GameObject> object);
			void commitChanges(std::string description);
		private:
			std::map<std::string,std::shared_ptr<Core::GameObject>> snapshots;
			std::shared_ptr<Core::GameObject> currentSnapshot = nullptr;
		};
	}
}

#endif