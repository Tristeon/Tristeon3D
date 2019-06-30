#include "Projection.h"

bool Tristan::Projection::overlap(Projection projection)
{
	if (min > projection.max) return false;
	if (max < projection.min) return false;
	return true;
}

float Tristan::Projection::getOverlap(Projection proj)
{
	if (proj.max < min) return 0;

	if (min >= proj.min)
		return proj.max - min;
	if (max < proj.min)
		return 0;

	return proj.max - proj.min;
}
