#include "HitResult.h"
#include <cmath>

HitResult::HitResult()
{
	hit = false;
	t = INFINITY;
	patch = NULL;
}