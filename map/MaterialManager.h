#pragma once

#include "../util/Vec3f.h"
#include <string>
#include <map>

class Game;

class NormalTable
{
public:
	static const int NORMALS_PER_DIRECTION = 20;

	Vec3f normals[6][NORMALS_PER_DIRECTION] ;

	Vec3f getRandomNormal( int dir );
	void printTable();
};


class Material
{
public:
	static const int NONE = 0;
	static const int OCEAN = 1;
	static const int GRASS = 2;
	static const int SAND = 3;
	static const int DIRT = 4;
	static const int ETHER = 5;
	static const int VOID = 6;
	static const int NUM_MATERIALS = 7;

	Material();

	std::string        name;
	int                id;
	Vec3f              debugColor;              		// grass is pure green, water pure blue, etc.

	Vec3f			   colors[3];       		        // three verts to barycenter between potentially.
	float              colorWeights[3];  				// barycenter weighting.
	float              normalThreshold;         		// 1.0 flat, 0.0 completely random within normal.

	bool               colorAnimMode;        			// on or off.
	float              colorAnimTimes[2];     			// min and max time.

	bool               normalAnimMode;          		// on or off
	float              normalAnimTimes[2];       		// min and max time

	Vec3f              randomColor();
	Vec3f              randomNormal( int dir );

	NormalTable      * normalTable;

	int                textureId;
};

// --------------------------------------------------------------------------
class MaterialManager
{
private:
	Game * mGame;

	std::map<float, NormalTable *>  mNormalTables;
	std::map<int, Material>  	    mMaterials;

public:

	void init( Game * game );

	void addMaterial( Material& mat );
	Material* material( int materialId );
};