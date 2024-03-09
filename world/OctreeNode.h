#pragma once

#include <vector>
#include "../util/Vec3i.h"
#include "../util/Vec3f.h"
#include "../util/Ray.h"
#include "../util/HitResult.h"

class Patch;

class OctreeNode 
{
private:
	Vec3f mMins;
	Vec3f mMaxs;
	Vec3f mCenter;
	std::vector<Patch *>  mPatches;
	OctreeNode*           mChildren[8];
	int                   mDirFlags;

	float size() { return mMaxs.x - mMins.x; }

public:
	constexpr static const float CELL_SIZE = 64.0f;

	OctreeNode( const Vec3f& mins, const Vec3f& maxs );
	void insertPatch( Patch *p );
	void removePatch( Patch *p );
	Patch * getPatchWithinRadius( const Vec3f& center, float radius );
	Patch * getPatchWithinLoc( World* world, const Vec3i& minLoc, const Vec3i& maxLoc, int materialId, int dirFlags );

	void rayTest( Ray& r, HitResult& h );

	static bool enclosesPatch( const Vec3f& mins, const Vec3f& maxs, Patch *p );
};