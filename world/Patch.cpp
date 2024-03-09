#include "Patch.h"
#include "../util/GameMath.h"
#include "../util/Vec3f.h"
#include "../game/Game.h"
#include "World.h"

Patch::Patch( World* world, const Vec3f& a, const Vec3f& b, const Vec3f& c, const Vec3f& baseCenter, int materialId )
{
	mWorld = world;

	verts[0].set( a );
	verts[1].set( b );
	verts[2].set( c );

	this->baseCenter.set( baseCenter );
	center.set( Vec3f::center3( a, b, c ) );
	baseNormal.set( Vec3f::cross( a, b, c ).normalized() );
	normal.set( baseNormal );
	dir = Vec3f::classify( baseNormal );

	resetAO();

	// do this after we know our direction.
	if( materialId != Material::NONE )
	{
		setMaterial( materialId );
	}
}

// -------------------------------------------------------------
void Patch::setMaterial( int materialId )
{
	this->materialId = materialId;

	Material * mat = mWorld->game()->materialManager()->material( materialId );

	color.set( mat->randomColor() );
	normal.set( mat->randomNormal( dir ) );

	colorAnimTime = 0.0f;
	colorAnimLength = 0.0f;
	normalAnimTime = 0.0f;
	normalAnimLength = 0.0f;

	if( materialId == Material::ETHER )
	{
		emissive.set( 1.0f, 1.0f, 1.0f );
		finalColor.set( emissive );
	}
	else
	{
		emissive.set( 0.0f, 0.0f, 0.0f );
		finalColor.set( color );
	}
}

// -------------------------------------------------------------
void Patch::resetAO()
{
	aoFree = 0;
	aoSamples = 0;
	ao = 0.0f;
}

// -------------------------------------------------------------
void Patch::tick( double dt )
{
	Material * mat = mWorld->game()->materialManager()->material( materialId );

	// *** animate colors ***
	if( mat->colorAnimMode  )
	{
		colorAnimTime += dt;
		if( colorAnimTime > colorAnimLength )
		{
			colorAnimTime = 0.0f;
			colorAnimLength = GameMath::randf( mat->colorAnimTimes[0], mat->colorAnimTimes[1] );

			colorAnimSource.set( color );
			colorAnimDest.set( mat->randomColor() );
		}
		else
		{
			float t = colorAnimTime / colorAnimLength;
			color.set( Vec3f::lerp( colorAnimSource, colorAnimDest, t ) );
		}
	}

	// *** animate normals ***
	if( mat->normalAnimMode  )
	{
		normalAnimTime += dt;
		if( normalAnimTime > normalAnimLength )
		{
			normalAnimTime = 0.0f;
			normalAnimLength = GameMath::randf( mat->normalAnimTimes[0], mat->normalAnimTimes[1] );

			normalAnimSource.set( normal );
			normalAnimDest.set( mat->randomNormal( dir ) );
		}
		else
		{
			float t = normalAnimTime / normalAnimLength;
			normal.set( Vec3f::slerp( normalAnimSource, normalAnimDest, t ) );
		}
	}
}

// // ---------------------------------------------------------------------
// bool Patch::contains( float x, float y, float z )
// {
// 	return GameMath::isPointInTriangle( verts[0], verts[1], verts[2], x, y, z );
// }
