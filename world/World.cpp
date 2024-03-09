#include "World.h"
#include "Patch.h"
#include "OctreeNode.h"
#include "../util/GameMath.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>

// for export to file.
#include <fstream>
#include <sstream>
#include <cassert>
#include "../game/Game.h"
#include "../resource/ResourceManager.h"
#include "../resource/TextureResource.h"
#include "../scene/editing/EditingScene.h"
#include "../scene/playing/PlayingScene.h"
#include "../util/Ray.h"
#include "../util/HitResult.h"

#include "../game/Constants.h"

World::World( Game *game, const Vec3f& mins, const Vec3f& maxs )
{
	mGame = game;

	mMins.set( mins );
	mMaxs.set( maxs );
	mRoot = new OctreeNode( mins, maxs );

	mShowBaseNormals = false;
	mShowNormals = false;

	mFullbright = false;
	mAmbientLightEnabled = true;
	mPointLightsEnabled = true;
	mDirLightsEnabled = true;
	mPaused = false;
}

// --------------------------------------------------
void World::clear()
{
	mPatches.clear();
	// TODO: fix mem leak here.
	mRoot = new OctreeNode( mMins, mMaxs );
}

// --------------------------------------------------
void World::addPatch( Patch *p )
{
	mPatches.push_back( p );
	mRoot->insertPatch( p );
}

// --------------------------------------------------
void World::removePatch( Patch* p )
{
	mRoot->removePatch( p );

	for( int i=mPatches.size() - 1; i >= 0; --i )
	{
		if( mPatches[i] == p )
		{
			 mPatches.erase(mPatches.begin() + i );
		}
	}
}

// --------------------------------------------------
Patch * World::getPatchAt( const Vec3f& center )
{
	return mRoot->getPatchWithinRadius( center, EDIT_RANGE );
}

// --------------------------------------------------
Patch * World::getPatchWithinRadius( const Vec3f& center, float radius )
{
	return mRoot->getPatchWithinRadius( center, radius );
}

// --------------------------------------------------
Patch * World::getPatchWithinLoc( const Vec3i& minLoc, const Vec3i& maxLoc, int materialId, int dirFlags )
{
	return mRoot->getPatchWithinLoc( this, minLoc, maxLoc, materialId, dirFlags );
}

// --------------------------------------------------
void World::addBlock( const Vec3i& loc, int materialId, int dirFlags )
{
	Vec3f center( (loc.i * BLOCK_SIZE) + HALF_BLOCK_SIZE,  
				  (loc.j * BLOCK_SIZE) + HALF_BLOCK_SIZE, 
				  (loc.k * BLOCK_SIZE) + HALF_BLOCK_SIZE );

	Vec3f a( center );  a.add( -HALF_BLOCK_SIZE, -HALF_BLOCK_SIZE, -HALF_BLOCK_SIZE );
	Vec3f b( center );  b.add( -HALF_BLOCK_SIZE, -HALF_BLOCK_SIZE, HALF_BLOCK_SIZE );
	Vec3f c( center );  c.add( HALF_BLOCK_SIZE, -HALF_BLOCK_SIZE, HALF_BLOCK_SIZE );
	Vec3f d( center );  d.add( HALF_BLOCK_SIZE, -HALF_BLOCK_SIZE, -HALF_BLOCK_SIZE );
	Vec3f e( center );  e.add( -HALF_BLOCK_SIZE, HALF_BLOCK_SIZE, -HALF_BLOCK_SIZE );
	Vec3f f( center );  f.add( -HALF_BLOCK_SIZE, HALF_BLOCK_SIZE, HALF_BLOCK_SIZE );
	Vec3f g( center );  g.add( HALF_BLOCK_SIZE, HALF_BLOCK_SIZE, HALF_BLOCK_SIZE );
	Vec3f h( center );  h.add( HALF_BLOCK_SIZE, HALF_BLOCK_SIZE, -HALF_BLOCK_SIZE );


	// *** erase any patches that are within this block ***
	Patch *p = NULL;
	while( (p = getPatchWithinRadius( center, HALF_BLOCK_SIZE * sqrt(2.0f) ) ) != NULL )
	{
		removePatch( p );
		p = NULL;
	}


	// bottom
	if( dirFlags & Vec3f::NEG_Y_DIR )
	{
		addPatch( new Patch( this, a, c, d, center.plus( 0.0f, -HALF_BLOCK_SIZE, 0.0f ), materialId ) );
		addPatch( new Patch( this, a, b, c, center.plus( 0.0f, -HALF_BLOCK_SIZE, 0.0f ), materialId  ) );
	}

	// left
	if( dirFlags & Vec3f::NEG_Z_DIR )
	{
		addPatch( new Patch( this, e, a, d, center.plus( 0.0f, 0.0f, -HALF_BLOCK_SIZE ), materialId  ) );
		addPatch( new Patch( this, e, d, h, center.plus( 0.0f, 0.0f, -HALF_BLOCK_SIZE ), materialId  ) );
	}

	// right
	if( dirFlags & Vec3f::NEG_X_DIR )
	{
		addPatch( new Patch( this, e, f, b, center.plus( -HALF_BLOCK_SIZE, 0.0f, 0.0f ), materialId  ) );
		addPatch( new Patch( this, e, b, a, center.plus( -HALF_BLOCK_SIZE, 0.0f, 0.0f ), materialId  ) );
	}

	// top
	if( dirFlags & Vec3f::POS_Y_DIR )
	{
		addPatch( new Patch( this, e, h, g, center.plus( 0.0f, HALF_BLOCK_SIZE, 0.0f ), materialId  ) );
		addPatch( new Patch( this, e, g, f, center.plus( 0.0f, HALF_BLOCK_SIZE, 0.0f ), materialId  ) );		
	}

	// back left
	if( dirFlags & Vec3f::POS_X_DIR )
	{
		addPatch( new Patch( this, c, h, d, center.plus( HALF_BLOCK_SIZE, 0.0f, 0.0f ), materialId  ) );
		addPatch( new Patch( this, c, g, h, center.plus( HALF_BLOCK_SIZE, 0.0f, 0.0f ), materialId  ) );		
	}

	// back right
	if( dirFlags & Vec3f::POS_Z_DIR )
	{
		addPatch( new Patch( this, c, f, g, center.plus( 0.0f, 0.0f, HALF_BLOCK_SIZE ), materialId  ) );
		addPatch( new Patch( this, c, b, f, center.plus( 0.0f, 0.0f, HALF_BLOCK_SIZE ), materialId  ) );
	}
}

// ------------------------------------------------------------
// for adjacency tests.
Vec3i World::patchLoc( Patch * p )
{
	return locFromPos( p->center.minus( p->baseNormal ) );
}

// ------------------------------------------------------------
// for picking
Vec3i World::locFromPos( const Vec3f& pos )
{
	// TODO: fix so that -1.0f, -1.0f, -1.0f becomes loc -1,-1,-1 not 0,0,0
	return Vec3i( (int) pos.x / BLOCK_SIZE, (int) pos.y / BLOCK_SIZE, (int) pos.z / BLOCK_SIZE );	
}

// --------------------------------------------------
// for rendering moves
Vec3f World::locToCenterPos( const Vec3i& loc )
{
	return Vec3f( loc.i * BLOCK_SIZE + HALF_BLOCK_SIZE, 
				  loc.j * BLOCK_SIZE + HALF_BLOCK_SIZE, 
				  loc.k * BLOCK_SIZE + HALF_BLOCK_SIZE );
}

 // --------------------------------------------------
bool World::lineOfSight( const Vec3f& start, const Vec3f& end )
{
	HitResult h;
	rayTest( start, end, h );
	return !h.hit;
}

// --------------------------------------------------
void World::rayTest( const Vec3f& start, const Vec3f& end, HitResult &h )
{
	Ray r( start, end );
	mRoot->rayTest( r, h );
}

// --------------------------------------------------
void World::tick( double dt )
{
	for( int i=0; i < mPatches.size(); ++i )
	{
		mPatches[i]->tick( dt );
	}
}

// --------------------------------------------------
Vec3f World::lightPatch( Camera& cam, Patch *p )
{
	// fullbright overrides all lighting.
	if( mFullbright )
	{
		return( Vec3f( 1.0f, 1.0f, 1.0f ) );
	}

	Vec3f out;

	if( p->emissive.x != 0.0f || p->emissive.y != 0.0f || p->emissive.z != 0.0f )
	{
		return( p->emissive );
	}

	Vec3f contactPoint = p->baseCenter.plus( p->baseNormal.times( 0.001 ) );
	// *** ambient lighting ***
	if( mAmbientLightEnabled && mLights[0].enabled )
	{
		// accumulate AO samples.
		if( p->aoSamples < 200 )
		{
			// trace a random ray.
			++(p->aoSamples);
			Vec3f n = Vec3f::cosWeightedHemisphere( p->baseNormal );
			if( lineOfSight( contactPoint, contactPoint.plus( n.times( WORLD_RANGE ) ) ) )
			{
				++ (p->aoFree);
			}
			p->ao = (float) p->aoFree / (float) p->aoSamples;
		}

		// ambient is just color
		out.add( mLights[0].color.times( p->ao ) );
	}	

	// *** direct lighting ***
	Vec3f direct;
	for( int i=1; i < MAX_LIGHTS; ++i )
	{
		Light& l = mLights[i];
		switch( l.type )
		{
			case Light::POINT:
			{
				if( mPointLightsEnabled && l.enabled )
				{
					Vec3f dirToLight( l.pos.minus( p->center ).normalized() );
					if( lineOfSight( contactPoint, l.pos ) )
					{
						// point light is  (color * intensity * nDotL) / distance^2
						Vec3f colorAtP( l.color );
						float dist = Vec3f::distance( p->center, l.pos );
						colorAtP.scale( l.intensity / ( dist * dist ) );

						Vec3f PL( l.pos.minus( p->center ).normalized() );
						colorAtP.scale( GameMath::maxf(Vec3f::dot( p->normal, PL ), 0.0f ) );
						direct.add( colorAtP );						
					}
				}
			} break;
			case Light::DIRECTIONAL:
			{
				if( mDirLightsEnabled && l.enabled )
				{
					Vec3f dirToLight( l.pos.minus( l.end ).normalized() );
					if( lineOfSight( contactPoint, contactPoint.plus( dirToLight.times( WORLD_RANGE ) ) ) )
					{
						// dirlight is (color * nDotL )
						Vec3f colorAtP( l.color );
						colorAtP.scale( GameMath::maxf(Vec3f::dot( p->normal, dirToLight ), 0.0f ) );
						direct.add( colorAtP );
					}
				}
			} break;
			default:
			{
				assert( false );
			} break;
		}
	}
	out.add( direct );

	return out;
}

// --------------------------------------------------
void World::render( Camera& cam )
{
	int lastMaterialId = -1;
	bool drawing = false;

	// *** patches ***
	glEnable( GL_TEXTURE_2D );
	glDisable( GL_BLEND );

	for( int i=0; i < mPatches.size(); ++i )
	{
		Patch *p = mPatches[i];
		// if visible,
		if( Vec3f::dot( cam.lookAt().inverse(), p->baseNormal ) > 0.0f )
		{
			// *** only change texture if we've changed material ***
			if( p->materialId != lastMaterialId )
			{
				if( drawing )
				{
					glEnd();
				}

				lastMaterialId = p->materialId;
				// set proper texture, or none
				Material* mat = mGame->materialManager()->material( p->materialId );
				if( mat->textureId == NO_TEXTURE )
				{
					glBindTexture( GL_TEXTURE_2D, 0 );
				}
				else
				{
					TextureResource* res = mGame->resourceManager()->texture( mat->textureId );
					glBindTexture( GL_TEXTURE_2D, res->textureId() );
				}

				drawing = true;
				glBegin( GL_TRIANGLES );
			}

			// light and render it.
			p->finalColor = lightPatch( cam, p ).modulate( p->color );

			// must call begin AFTER binding textures
			glColor3f( p->finalColor.x, p->finalColor.y, p->finalColor.z );
			glTexCoord2f( 0.0f, 1.0f );
			glVertex3f( p->verts[0].x, p->verts[0].y, p->verts[0].z );

			glTexCoord2f( 1.0f, 1.0f );
			glVertex3f( p->verts[1].x, p->verts[1].y, p->verts[1].z );

			glTexCoord2f( 1.0f, 0.0f );
			glVertex3f( p->verts[2].x, p->verts[2].y, p->verts[2].z );
		}
	}

	if( drawing )
	{
		glEnd();
	}

	glDisable( GL_TEXTURE_2D );


	// *** FACE NORMALS ***
	if( mShowBaseNormals )
	{
		glBegin( GL_LINES );
		for( int i=0; i < mPatches.size(); ++i )
		{
			glColor3f( mPatches[i]->finalColor.x, mPatches[i]->finalColor.y, mPatches[i]->finalColor.z );
			glVertex3f( mPatches[i]->center.x, mPatches[i]->center.y, mPatches[i]->center.z );

			glColor3f( 1.0f, 1.0f, 1.0f );
			glVertex3f( mPatches[i]->center.x + mPatches[i]->baseNormal.x * 20.0f,
					    mPatches[i]->center.y + mPatches[i]->baseNormal.y * 20.0f, 
					    mPatches[i]->center.z + mPatches[i]->baseNormal.z * 20.0f);
		}
		glEnd();		
	}


	if( mShowNormals )
	{
		glBegin( GL_LINES );
		for( int i=0; i < mPatches.size(); ++i )
		{
			glColor3f( mPatches[i]->finalColor.x, mPatches[i]->finalColor.y, mPatches[i]->finalColor.z );
			glVertex3f( mPatches[i]->center.x, mPatches[i]->center.y, mPatches[i]->center.z );

			glColor3f( 1.0f, 1.0f, 1.0f );
			glVertex3f( mPatches[i]->center.x + mPatches[i]->normal.x * 20.0f,
					    mPatches[i]->center.y + mPatches[i]->normal.y * 20.0f, 
					    mPatches[i]->center.z + mPatches[i]->normal.z * 20.0f);
		}
		glEnd();
	}
}

// --------------------------------------------------
void World::writeToFile( const char * path )
{
  	std::ostringstream ss;
 
  	ss << std::endl;
  	ss << "clear" << std::endl;

  	for( int i=0; i < mPatches.size(); ++i )
  	{
  		Patch *p = mPatches[i];
	  	ss << "addpatch " << p->verts[0].x << " " << p->verts[0].y << " " << p->verts[0].z << " "
 			<< p->verts[1].x << " " << p->verts[1].y << " " << p->verts[1].z << " "
 			<< p->verts[2].x << " " << p->verts[2].y << " " << p->verts[2].z << " "
 			<< p->baseCenter.x << " " << p->baseCenter.y << " " << p->baseCenter.z << " " << p->materialId << std::endl;
  	}

    for( int i=0; i < MAX_LIGHTS; ++i )
    {
    	const Light& l = mLights[i];

		if( l.enabled )
		{
  			ss << "lightenabled " << i << " 1"  << std::endl;
  		}
  		else
  		{
  			ss << "lightenabled " << i << " 0"  << std::endl;
  		}

  		ss << "lighttype " << i << " " << l.type << std::endl;
	  	ss << "lightintensity " << i << " " << l.intensity << std::endl;
	  	ss << "lightrgb " << i << " " << l.color.x << " " << l.color.y << " " << l.color.z << std::endl;
	  	ss << "lightpos " << i << " " << l.pos.x << " " << l.pos.y << " " << l.pos.z << std::endl;
	  	ss << "lightend " << i << " " << l.end.x << " " << l.end.y << " " << l.end.z << std::endl;
    }

  	ss << std::endl;

  	// *** save camera states ***
  	Camera& editCam = mGame->editingScene()->camera();
  	const Vec3f& editCamPos = editCam.pos();
  	ss << "editcam " << editCamPos.x << " " << editCamPos.y << " " << editCamPos.z << " " << editCam.yaw() << " " << editCam.pitch() << std::endl;

  	Camera& playCam = mGame->playingScene()->camera();
  	const Vec3f& playCamPos = playCam.pos();
  	ss << "playcam " << playCamPos.x << " " << playCamPos.y << " " << playCamPos.z << " " << playCam.yaw() << " " << playCam.pitch() << " " << playCam.zoom() << std::endl;


  	// *** save backdrop states ***
  	ss << "backdropclear" << std::endl;
    ss << "backdrop " << ( mGame->backdrop().visible() ? "1" : "0" ) << std::endl;
    for( int i=0; i < mGame->backdrop().stopCount(); ++i )
    {
	    ss << "backdropadd " << mGame->backdrop().stop(i).percent << " " << mGame->backdrop().stop(i).color.x << " " << mGame->backdrop().stop(i).color.y << " "<< mGame->backdrop().stop(i).color.z << std::endl;
    }
  
  	// *** write map to file ***
    std::ofstream ofs;
    ofs.open(path);
    ofs << ss.str();
    ofs.close();

    printf("Wrote map to file %s\n", path );
}