#include "MaterialManager.h"
#include "../util/GameMath.h"
#include "../game/Constants.h"
#include <cassert>

Vec3f NormalTable::getRandomNormal( int dir )
{
	int dirIndex = -1;
	while( dir > 0 )
	{
		dir /= 2;
		++dirIndex;
	}

	assert( dirIndex >= 0 && dirIndex <= 5 );

	int i = GameMath::randi( 0, NORMALS_PER_DIRECTION - 1);
	assert( i >= 0 && i < NORMALS_PER_DIRECTION );

	return normals[dir][i];
}

// --------------------------------------------------------------------------
void NormalTable::printTable()
{
	for( int i=0; i < 6; ++i )
	{
		for( int j=0; j < NORMALS_PER_DIRECTION; ++j )
		{
			printf("%d %d %f %f %f\n", i, j, normals[i][j].x, normals[i][j].y, normals[i][j].z );
		}
	}
}

// --------------------------------------------------------------------------
Material::Material()
{
	name       = "";
	id         = -1;
	debugColor.setHex( 0xffffff );

	colors[0].setHex( 0xffffff );  colors[1].setHex( 0xffffff );  colors[2].setHex( 0xffffff );
	colorWeights[0] = 1.0f;        colorWeights[1] = 1.0f;        colorWeights[2] = 1.0f;
	normalThreshold = 0.0f;

	colorAnimMode = false;
	colorAnimTimes[0] = 0.0f;      colorAnimTimes[1] = 0.0f;

	normalAnimMode = false;
	normalAnimTimes[0] = 0.0f;     normalAnimTimes[1] = 0.0f;

	normalTable = NULL;
}

// --------------------------------------------------------------------------
Vec3f Material::randomColor()
{
	//return colors[0];
	return Vec3f::lerp( colors[0], colors[1], GameMath::randf( 0.0f, 1.0f ) );
	//return GameMath::randomPointInTriangle( colors[0], colors[1], colors[2] );
}

// --------------------------------------------------------------------------
Vec3f Material::randomNormal( int dir )
{
	// make sure we're not an unknown dir.
	assert( dir > 0 );

	if( normalThreshold == 1.0f )
	{
		switch( dir )
		{
			case Vec3f::POS_Y_DIR:
			{
				return Vec3f::POS_Y;
			} break;
			case Vec3f::NEG_Y_DIR:
			{
				return Vec3f::NEG_Y;
			} break;
			case Vec3f::POS_X_DIR:
			{
				return Vec3f::POS_X;
			} break;
			case Vec3f::NEG_X_DIR:
			{
				return Vec3f::NEG_X;
			} break;
			case Vec3f::POS_Z_DIR:
			{
				return Vec3f::POS_Z;
			} break;
			case Vec3f::NEG_Z_DIR:
			{
				return Vec3f::NEG_Z;
			} break;
			default:
			{
				assert( false );
			}
		}
	}
	else
	{
		return normalTable->getRandomNormal( dir );
	}
}

// --------------------------------------------------------------------------
void MaterialManager::init( Game * game )
{
	mGame = game;
	Material mat;

	// ---

	mat.name       = "none";
	mat.id         = Material::NONE;
	mat.debugColor.setHex( 0xff00ff );

	mat.colors[0].setHex( 0xff00ff );  mat.colors[1].setHex( 0xff00ff );  mat.colors[2].setHex( 0xff00ff );
	mat.colorWeights[0] = 1.0f;        mat.colorWeights[1] = 1.0f;        mat.colorWeights[2] = 1.0f;
	mat.normalThreshold = 1.0f;

	mat.colorAnimMode = false;
	mat.colorAnimTimes[0] = 0.0f;      mat.colorAnimTimes[1] = 0.0f;

	mat.normalAnimMode = false;
	mat.normalAnimTimes[0] = 0.0f;     mat.normalAnimTimes[1] = 0.0f;

	mat.textureId = NO_TEXTURE;

	addMaterial( mat );

	// ---

	mat.name       = "ocean";
	mat.id         = Material::OCEAN;
	mat.debugColor.setHex( 0x04b6f7 );

	mat.colors[0].setHex( 0x04b6f7 );  mat.colors[1].setHex( 0x04b6f7 );  mat.colors[2].setHex( 0x04b6f7 );
	mat.colorWeights[0] = 1.0f;        mat.colorWeights[1] = 1.0f;        mat.colorWeights[2] = 1.0f;
	mat.normalThreshold = 0.99f;

	mat.colorAnimMode = false;
	mat.colorAnimTimes[0] = 0.0f;      mat.colorAnimTimes[1] = 0.0f;

	mat.normalAnimMode = true;
	mat.normalAnimTimes[0] = 0.5f;    mat.normalAnimTimes[1] = 1.5f;

	mat.textureId = OCEAN_TEXTURE;

	addMaterial( mat );

	// ---

	mat.name       = "grass";
	mat.id         = Material::GRASS;
	mat.debugColor.setHex( 0xbbe03b );

	mat.colors[0].setHex( 0xbbe03b );  mat.colors[1].setHex( 0xbbe03b );  mat.colors[2].setHex( 0xbbe03b );
	mat.colorWeights[0] = 1.0f;        mat.colorWeights[1] = 1.0f;        mat.colorWeights[2] = 1.0f;
	mat.normalThreshold = 0.999f;

	mat.colorAnimMode = false;
	mat.colorAnimTimes[0] = 0.2f;      mat.colorAnimTimes[1] = 4.0f;

	mat.normalAnimMode = false;
	mat.normalAnimTimes[0] = 2.0f;     mat.normalAnimTimes[1] = 4.0f;

	mat.textureId = GRASS_TEXTURE;

	addMaterial( mat );

	// ---

	mat.name       = "dirt";
	mat.id         = Material::DIRT;
	mat.debugColor.setHex( 0xdd9b49 );

	mat.colors[0].setHex( 0xdd9b49 );  mat.colors[1].setHex( 0xdd9b49 );  mat.colors[2].setHex( 0xdd9b49 );
	mat.colorWeights[0] = 1.0f;        mat.colorWeights[1] = 1.0f;        mat.colorWeights[2] = 1.0f;
	mat.normalThreshold = 0.99f;

	mat.colorAnimMode = false;
	mat.colorAnimTimes[0] = 0.2f;      mat.colorAnimTimes[1] = 1.0f;

	mat.normalAnimMode = false;
	mat.normalAnimTimes[0] = 2.0f;     mat.normalAnimTimes[1] = 4.0f;

	mat.textureId = DIRT_TEXTURE;

	addMaterial( mat );

	// ---

	mat.name       = "sand";
	mat.id         = Material::SAND;
	mat.debugColor.setHex( 0xf5daad );

	mat.colors[0].setHex( 0xf5daad );  mat.colors[1].setHex( 0xf5daad );  mat.colors[2].setHex( 0xf5daad );
	mat.colorWeights[0] = 1.0f;        mat.colorWeights[1] = 1.0f;        mat.colorWeights[2] = 1.0f;
	mat.normalThreshold = 0.995f;

	mat.colorAnimMode = false;
	mat.colorAnimTimes[0] = 0.2f;      mat.colorAnimTimes[1] = 1.0f;

	mat.normalAnimMode = false;
	mat.normalAnimTimes[0] = 2.0f;     mat.normalAnimTimes[1] = 4.0f;

	mat.textureId = SAND_TEXTURE;

	addMaterial( mat );

	// ---

	mat.name       = "ether";
	mat.id         = Material::ETHER;
	mat.debugColor.set( Vec3f::WHITE );

	mat.colors[0].set( Vec3f::WHITE );  mat.colors[1].set( Vec3f::WHITE );  mat.colors[2].set( Vec3f::WHITE );
	mat.colorWeights[0] = 1.0f;         mat.colorWeights[1] = 1.0f;         mat.colorWeights[2] = 1.0f;
	mat.normalThreshold = 1.0f;

	mat.colorAnimMode = false;
	mat.colorAnimTimes[0] = 0.2f;      mat.colorAnimTimes[1] = 1.0f;

	mat.normalAnimMode = false;
	mat.normalAnimTimes[0] = 2.0f;     mat.normalAnimTimes[1] = 4.0f;

	mat.textureId = NO_TEXTURE;

	addMaterial( mat );


	// ---

	mat.name       = "void";
	mat.id         = Material::VOID;
	mat.debugColor.set( Vec3f::DARK_VIOLET );

	mat.colors[0].set( Vec3f::DARK_VIOLET );  mat.colors[1].set( Vec3f::BLACK );  mat.colors[2].set( Vec3f::BLACK );
	mat.colorWeights[0] = 1.0f;        		  mat.colorWeights[1] = 1.0f;        mat.colorWeights[2] = 1.0f;
	mat.normalThreshold = 1.0f;

	mat.colorAnimMode = true;
	mat.colorAnimTimes[0] = 2.0f;      mat.colorAnimTimes[1] = 4.0f;

	mat.normalAnimMode = false;
	mat.normalAnimTimes[0] = 2.0f;     mat.normalAnimTimes[1] = 4.0f;

	mat.textureId = NO_TEXTURE;

	addMaterial( mat );
}

// ---------------------------------------------------------------------
// builds a normal table within the threshold if one doesn't exist.
void MaterialManager::addMaterial( Material &mat )
{
	assert( mMaterials.find( mat.id ) == mMaterials.end() );

	NormalTable * n = NULL;
	// haven't encountered this threshold yet.
	if( mNormalTables.find( mat.normalThreshold ) == mNormalTables.end() )
	{
		NormalTable *n = new NormalTable();
		for( int i=0; i < NormalTable::NORMALS_PER_DIRECTION; ++i )
		{
			n->normals[Vec3f::POS_Y_DIR - 1][i] = Vec3f::cosWeightedHemisphereWithin( Vec3f::POS_Y, mat.normalThreshold );
			n->normals[Vec3f::NEG_Y_DIR - 1][i] = Vec3f::cosWeightedHemisphereWithin( Vec3f::NEG_Y, mat.normalThreshold );
			n->normals[Vec3f::POS_X_DIR - 1][i] = Vec3f::cosWeightedHemisphereWithin( Vec3f::POS_X, mat.normalThreshold );
			n->normals[Vec3f::NEG_X_DIR - 1][i] = Vec3f::cosWeightedHemisphereWithin( Vec3f::NEG_X, mat.normalThreshold );
			n->normals[Vec3f::POS_Z_DIR - 1][i] = Vec3f::cosWeightedHemisphereWithin( Vec3f::POS_Z, mat.normalThreshold );
			n->normals[Vec3f::NEG_Z_DIR - 1][i] = Vec3f::cosWeightedHemisphereWithin( Vec3f::NEG_Z, mat.normalThreshold );
		}
		mNormalTables[mat.normalThreshold] = n;
		//n->printTable();
	}		

	// mat references normal table for animation
	mat.normalTable = mNormalTables[mat.normalThreshold];

	mMaterials[ mat.id ] = mat;
}

// ---------------------------------------------------------------------
Material * MaterialManager::material( int materialId )
{
	return &mMaterials[materialId];
}