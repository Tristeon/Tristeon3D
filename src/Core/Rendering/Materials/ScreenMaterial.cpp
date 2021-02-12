#include "ScreenMaterial.h"

namespace Tristeon::Core::Rendering
{
	REGISTER_TYPE_CPP(ScreenMaterial);

	ShaderFile* ScreenMaterial::shader()
	{
		static ShaderFile shader("Files/Shaders/Color.vert.spv", "Files/Shaders/Color.frag.spv");
		return &shader;
	}
}