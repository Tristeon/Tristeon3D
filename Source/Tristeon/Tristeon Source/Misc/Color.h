#pragma once
#include "Core/TObject.h"

namespace Tristeon
{
	namespace Misc
	{
		/**
		 * \brief Color is a class describing a color by using 4 float values (rgba)
		 */
		class Color final : public Core::TObject
		{
		public:
			/**
			 * \brief Creates a white color (1, 1, 1, 1)
			 */
			Color();
			/**
			 * \brief Creates a color with the given parameters
			 * \param r The red component of the color
			 * \param g The green component of the color
			 * \param b The blue component of the color
			 * \param a The alpha component of the color
			 */
			Color(float r, float g, float b, float a);
			/**
			 * \brief Creates a color with the given color's values
			 * \param c The color to be copied
			 */
			Color(const Color& c);

			/**
			 * \brief Serializes the color into a json file
			 * \return The json file describing this color
			 */
			nlohmann::json serialize() override; 
			nlohmann::json serialize_const() const;
			/**
			 * \brief Deserializes the color from a json file
			 * \param json The json file describing the color
			 */
			void deserialize(nlohmann::json json) override;

			/**
			 * \brief Converts the color data to an array of 4 floats
			 */
			std::array<float, 4> toArray() const { return {r,g,b,a}; }

			/**
			 * \brief The red component of the color
			 */
			float r;
			/**
			 * \brief The green component of the color
			 */
			float g;
			/**
			 * \brief The blue component of the color
			 */
			float b;
			/**
			 * \brief The alpha component of the color
			 */
			float a;
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
