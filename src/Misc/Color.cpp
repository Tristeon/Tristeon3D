#include "Color.h"
#include "XPlatform/typename.h"

namespace Tristeon
{
	namespace Misc
	{
		Color::Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) { }
		Color::Color(const Color& c) : r(c.r), g(c.g), b(c.b), a(c.a) { }

		nlohmann::json Color::serialize()
		{
			return serialize_const();
		}

		nlohmann::json Color::serialize_const() const
		{
			nlohmann::json j;
			j["typeID"] = TRISTEON_TYPENAME(Color);
			j["r"] = r;
			j["g"] = g;
			j["b"] = b;
			j["a"] = a;
			return j;
		}

		void Color::deserialize(nlohmann::json json)
		{
			r = json["r"];
			g = json["g"];
			b = json["b"];
			a = json["a"];
		}
	}
}