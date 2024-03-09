#include "OctreeNode.h"
#include "Patch.h"
#include "../util/GameMath.h"
#include "../world/World.h"

OctreeNode::OctreeNode( const Vec3f& mins, const Vec3f& maxs )
{
	mMins.set( mins );
	mMaxs.set( maxs );
	mCenter.set( mMaxs.plus( mMins ).times( 0.5 ) );

	for( int i=0; i < 8; ++i )
		mChildren[i] = NULL;
}

// ------------------------------------------------------------
void OctreeNode::insertPatch( Patch *p )
{
	mDirFlags |= p->dir;

	//printf("attempting to insert into %0.2f %0.2f %0.2f to %0.2f %0.2f %0.2f size %0.2f\n", mMins.x, mMins.y, mMins.z, mMaxs.x, mMaxs.y, mMaxs.z, size() );
	// *** can we go no smaller? ***
	if( size() <= CELL_SIZE )
	{
		mPatches.push_back( p );
		return;
	}


	Vec3f testMins;
	Vec3f testMaxs;
	//bool inserted = false;

	// bottom octants
	testMins.set( mMins.x, mMins.y, mMins.z );
	testMaxs.set( mCenter.x, mCenter.y, mCenter.z );
	if( OctreeNode::enclosesPatch( testMins, testMaxs, p ) )
	{
		if( !mChildren[0] ) mChildren[0] = new OctreeNode( testMins, testMaxs );
		mChildren[0]->insertPatch( p );
		return;
		//inserted = true;
	}

	testMins.set( mMins.x, mMins.y, mCenter.z );
	testMaxs.set( mCenter.x, mCenter.y, mMaxs.z );
	if( OctreeNode::enclosesPatch( testMins, testMaxs, p ) )
	{
		if( !mChildren[1] ) mChildren[1] = new OctreeNode( testMins, testMaxs );
		mChildren[1]->insertPatch( p );
		return;
		//inserted = true;
	}

	testMins.set( mCenter.x, mMins.y, mMins.z );
	testMaxs.set( mMaxs.x, mCenter.y, mCenter.z );
	if( OctreeNode::enclosesPatch( testMins, testMaxs, p ) )
	{
		if( !mChildren[2] ) mChildren[2] = new OctreeNode( testMins, testMaxs );
		mChildren[2]->insertPatch( p );
		return;
		//inserted = true;
	}

	testMins.set( mCenter.x, mMins.y, mCenter.z );
	testMaxs.set( mMaxs.x, mCenter.y, mMaxs.z );
	if( OctreeNode::enclosesPatch( testMins, testMaxs, p ) )
	{
		if( !mChildren[3] ) mChildren[3] = new OctreeNode( testMins, testMaxs );
		mChildren[3]->insertPatch( p );
		return;
		//inserted = true;
	}


	// top octants
	testMins.set( mMins.x, mCenter.y, mMins.z );
	testMaxs.set( mCenter.x, mMaxs.y, mCenter.z );
	if( OctreeNode::enclosesPatch( testMins, testMaxs, p ) )
	{
		if( !mChildren[4] ) mChildren[4] = new OctreeNode( testMins, testMaxs );
		mChildren[4]->insertPatch( p );
		return;
		//inserted = true;
	}

	testMins.set( mMins.x, mCenter.y, mCenter.z );
	testMaxs.set( mCenter.x, mMaxs.y, mMaxs.z );
	if( enclosesPatch( testMins, testMaxs, p ) )
	{
		if( !mChildren[5] ) mChildren[5] = new OctreeNode( testMins, testMaxs );
		mChildren[5]->insertPatch( p );
		return;
		//inserted = true;
	}

	testMins.set( mCenter.x, mCenter.y, mMins.z );
	testMaxs.set( mMaxs.x, mMaxs.y, mCenter.z );
	if( OctreeNode::enclosesPatch( testMins, testMaxs, p ) )
	{
		if( !mChildren[6] ) mChildren[6] = new OctreeNode( testMins, testMaxs );
		mChildren[6]->insertPatch( p );
		return;
		//inserted = true;
	}

	testMins.set( mCenter.x, mCenter.y, mCenter.z );
	testMaxs.set( mMaxs.x, mMaxs.y, mMaxs.z );
	if( OctreeNode::enclosesPatch( testMins, testMaxs, p ) )
	{
		if( !mChildren[7] ) mChildren[7] = new OctreeNode( testMins, testMaxs );
		mChildren[7]->insertPatch( p );
		return;
		//inserted = true;
	}


	//if( inserted ) return;

	// *** we should never have anything yet that spans a node ***
	//printf("WARNING: inserted a spanning patch!\n");
	mPatches.push_back( p );
}

// ------------------------------------------------------------
void OctreeNode::removePatch( Patch *p )
{
	for( int i=mPatches.size() - 1; i >= 0; --i )
	{
		if( mPatches[i] == p )
		{
			mPatches.erase(mPatches.begin() + i );
		}
	}

	for( int i=0; i < 8; ++i )
	{
		if( mChildren[i] )
		{
			mChildren[i]->removePatch( p );
		}
	}
}

// ------------------------------------------------------------
Patch * OctreeNode::getPatchWithinRadius( const Vec3f& center, float radius ) 
{
	for( int i=0; i < mPatches.size(); ++i )
	{
		if( Vec3f::distance( mPatches[i]->center, center ) <= radius )
		{
			return mPatches[i];
		}
	}

	// got here, need to check children.
	Patch * patch = NULL;
	for( int i=0; i < 8; ++i )
	{
		if( mChildren[i] )
		{
			patch = mChildren[i]->getPatchWithinRadius( center, radius );
			if( patch )
			{
				return patch;
			}	
		}
	}

	// got this far, no one had it.
	return NULL;
}

// ------------------------------------------------------------
// to find an ether around a starting patch.
Patch* OctreeNode::getPatchWithinLoc( World* world, const Vec3i& minLoc, const Vec3i& maxLoc, int materialId, int dirFlags )
{
	//printf("Get adjacent patch %d %d %d %d %d %d %d %d\n", minLoc.i, minLoc.j, minLoc.k, maxLoc.i, maxLoc.j, maxLoc.k, materialId, dir );

	for( int i=0; i < mPatches.size(); ++i )
	{
		Vec3i loc = world->patchLoc( mPatches[i] );
		if( loc.i >= minLoc.i && loc.i <= maxLoc.i &&
			loc.j >= minLoc.j && loc.j <= maxLoc.j &&
			loc.k >= minLoc.k && loc.k <= maxLoc.k &&
			mPatches[i]->materialId == materialId &&
			(mPatches[i]->dir & dirFlags ) )
		{
			return mPatches[i];
		}
	}

	Patch* patch = NULL;
	for( int i=0; i < 8; ++i )
	{
		if( mChildren[i] )
		{
			patch = mChildren[i]->getPatchWithinLoc( world, minLoc, maxLoc, materialId, dirFlags );
			if( patch )
			{
				return patch;
			}	
		}
	}

	return NULL;
}

// ------------------------------------------------------------
void OctreeNode::rayTest( Ray& r, HitResult& h )
{
	// TODO: limit by stepping increase of t, instead of checking every AABB regardless of distance.
	if( !( mDirFlags & r.hitFlags ) ) return;
	float t;
	if( !GameMath::rayInAABB( r, mMins, mMaxs, t ) ) return;

	for( int i=0; i < mPatches.size(); ++i )
	{
		if( ( r.hitFlags & mPatches[i]->dir ) && GameMath::rayInTriangle( r, mPatches[i]->verts[0], mPatches[i]->verts[1], mPatches[i]->verts[2], t ) )
		{
			if( t < h.t )
			{
				h.hit = true;
				h.t = t;
				h.patch = mPatches[i];
			}
		}
	}	

	for( int i=0; i < 8; ++i )
	{
		if( mChildren[i] )
			mChildren[i]->rayTest( r, h );
	}
}

// ------------------------------------------------------------
bool OctreeNode::enclosesPatch( const Vec3f& mins, const Vec3f& maxs, Patch *p )
{
	// any vert outside, fails
	for( int i=0; i < 3; ++i )
	{
		if( !GameMath::pointInAABB( p->verts[i], mins, maxs ) ) return false;
	}

	// if( ! GameMath::pointInAABB( p->center.minus( p->baseNormal ), mins, maxs ) ) return false;

	return true;
}