#include "Projection.h"

bool Tristan::Projection::overlap(Projection projection)
{
	if (min > projection.max) return false;
	if (max < projection.min) return false;
	return true;
}