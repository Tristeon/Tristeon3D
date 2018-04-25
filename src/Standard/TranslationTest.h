#pragma once
#include "Core/Components/Component.h"

namespace Tristeon
{
	namespace Standard
	{
		class TranslationTest : public Core::Components::Component
		{
		public:
			void start() override;
			void update() override;
		};
	}
}
