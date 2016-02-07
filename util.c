#include "util.h"
#include <stdlib.h>

double myRandom(double min, double max)
{
	double	d;

	/* return a random number uniformly draw from [min,max] */
	d = min+(max-min)*(rand()%0x7fff)/32767.0;

	return d;
}
