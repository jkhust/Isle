#pragma once

#include "../world/Patch.h"

class HitResult
{
public:
	bool    hit;
	float   t;
	Patch * patch;

	HitResult();
	
};