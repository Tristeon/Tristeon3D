#pragma once

namespace Tristan
{
	class Projection
	{
	public:
		Projection(float min, float max) : min(min), max(max) {}

		float min;
		float max;

		bool overlap(Projection projection);

		float getOverlap(Projection proj);
	};
}