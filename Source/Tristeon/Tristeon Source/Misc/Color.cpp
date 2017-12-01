#include "Color.h"

namespace Tristeon
{
	namespace Misc
	{
		Color::Color() : Color(1, 1, 1, 1)
		{
			//White color
		}

		Color::Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a)
		{
			//Color from parameters
		}

		Color::Color(const Color& c) : r(c.r), g(c.g), b(c.b), a(c.a)
		{
			//Color from c's values
		}

		nlohmann::json Color::serialize()
		{
			//Serialize into a json file
			nlohmann::json j;
			j["typeID"] = typeid(Color).name();
			j["r"] = r;
			j["g"] = g;
			j["b"] = b;
			j["a"] = a;
			return j;
		}

		void Color::deserialize(nlohmann::json json)
		{
			//Deserialize from a json file
			r = json["r"];
			g = json["g"];
			b = json["b"];
			a = json["a"];
		}
	}
}
