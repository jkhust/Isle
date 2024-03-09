#pragma once

#include <vector>
#include "../scene/Camera.h"
#include "../util/Vec3f.h"
#include "../util/Vec3i.h"
#include "../util/Ray.h"
#include "../util/HitResult.h"
#include "Light.h"

class Game;
class Patch;
class OctreeNode;

const int   MAX_LIGHTS = 10;

class World
{
private:
	Game *               mGame;
	Light 				 mLights[MAX_LIGHTS];
	std::vector<Patch *> mPatches;

	Vec3f                mMins;
	Vec3f                mMaxs;
	OctreeNode *         mRoot;

	// *** developer toggles ***
	bool mShowBaseNormals;
	bool mShowNormals;

	bool mFullbright;			// overrides all other lighting settings.
	bool mAmbientLightEnabled;
	bool mPointLightsEnabled;
	bool mDirLightsEnabled;
	bool mPaused;

	Vec3f lightPatch( Camera& cam, Patch *p );

public:
	constexpr static const float WORLD_EXTENT = 1024.0f;
	constexpr static const float WORLD_RANGE = WORLD_EXTENT * 2.0f * 1.41421356237f;   // world width mins to maxs
	constexpr static const float BLOCK_SIZE   = 32.0f;     //width, height, depth
	constexpr static const float HALF_BLOCK_SIZE = BLOCK_SIZE / 2.0f;
	constexpr static const float EDIT_RANGE = 12.0f;

	Game* game() { return mGame; }
	Light* getLight( int index ) 			{ return &mLights[index]; }
	std::vector<Patch *> & patches()        { return mPatches; }

	void showBaseNormals( bool value )		{ mShowBaseNormals = value; }
	void showNormals( bool value )			{ mShowNormals = value; }
	void setFullbright( bool value )		{ mFullbright = value; }
	void enableAmbientLight( bool value )   { mAmbientLightEnabled = value; }
	void enablePointLights( bool value )    { mPointLightsEnabled = value; }
	void enableDirLights( bool value )    	{ mDirLightsEnabled = value; }
	bool paused()    					    { return mPaused; }
	void setPaused( bool value )    		{ mPaused = value; }


	World( Game* game, const Vec3f& mins, const Vec3f& maxs );
	void clear();
	void addPatch( Patch *p );
	void removePatch( Patch *p );
	Patch * getPatchAt( const Vec3f& center );
	Patch * getPatchWithinRadius( const Vec3f& center, float radius );
	Patch * getPatchWithinLoc( const Vec3i& minLoc, const Vec3i& maxLoc, int materialId, int dirFlags );

	void addBlock( const Vec3i& loc, int materialId, int dirFlags = Vec3f::ALL_DIRS );
	Vec3i patchLoc( Patch *p );
	Vec3i locFromPos( const Vec3f& pos );
	Vec3f locToCenterPos( const Vec3i& loc );

	bool lineOfSight( const Vec3f& start, const Vec3f& end );
	void rayTest( const Vec3f& start, const Vec3f& end, HitResult &h );

	void tick( double dt );
	void render( Camera& cam );

	void writeToFile( const char *path );
};