#pragma once

#include "../util/Vec3f.h"
#include "../map/MaterialManager.h"

class World;

class Patch
{
public:
	World * mWorld;

	// supplied
	Vec3f verts[3];
	Vec3f baseCenter;  // center of cube face
	int   materialId;

	// calculated.
	Vec3f center;
	Vec3f baseNormal;
	int   dir;

 	// *** appearance and gameplay data *** 
	Vec3f    color;
	Vec3f    normal;

	// *** instance vars for material animation ***
	float 	 colorAnimTime;
	float    colorAnimLength;
	Vec3f    colorAnimSource;
	Vec3f    colorAnimDest;

	// *** instance vars for normal animation ***
	float 	 normalAnimTime;
	float    normalAnimLength;
	Vec3f    normalAnimSource;
	Vec3f    normalAnimDest;

	// *** shading data ***
	Vec3f    emissive;

	int      aoSamples;
	int      aoFree;
	float    ao;
	Vec3f    finalColor;


	Patch( World* world, const Vec3f& a, const Vec3f& b, const Vec3f& c, const Vec3f& baseCenter, int materialId = Material::NONE );
	void setMaterial( int materialId );

	void resetAO();
	void tick( double dt );
};