#pragma once

#include "Vec3f.h"

class Ray
{
public:
	Vec3f start;
	Vec3f end;
	Vec3f dir;
	Vec3f dir_inv;
	int   hitFlags;

	Ray( const Vec3f& start, const Vec3f& end );
	
};