#pragma once
#include "Core/TObject.h"

namespace Tristeon
{
	namespace Misc
	{
		/**
		 * Color is a structure used to describe a color by using 4 float values (rgba)
		 */
		struct Color final : Core::TObject
		{
			explicit Color(float r = 1, float g = 1, float b = 1, float a = 1);
			Color(const Color& c);

			/**
			 * Converts the color data to an array of 4 floats
			 */
			std::array<float, 4> toArray() const { return { r, g, b, a}; }

			std::string toString() const { return "{ " + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ", " + std::to_string(a) + "}"; }

			float r;
			float g;
			float b;
			float a;

			nlohmann::json serialize() override; 
			nlohmann::json serialize_const() const;
			void deserialize(nlohmann::json json) override;
		};

		//Override functions for json maps/vectors
		inline void to_json(nlohmann::json& j, const Color& p) {
			j = p.serialize_const();
		}

		inline void from_json(const nlohmann::json& j, Color& p) {
			p.deserialize(j);
		}
	}
}
