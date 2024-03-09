#pragma once

#include "../util/Vec3f.h"

class Light
{
public:
	static const int AMBIENT = 0;
	static const int POINT = 1;
	static const int DIRECTIONAL = 2;

	bool  enabled;
	Vec3f color;
	float intensity;
	Vec3f pos;
	Vec3f end;
	int type;

	Light();
};