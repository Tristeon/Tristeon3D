#include "Projection.h"

bool Tristan::Projection::overlap(Projection projection)
{
	if (max < projection.min) return false;
	if (min > projection.max) return false;
	//If the projection is not outside the min/max boundaries, there must be overlap
	return true;
}
