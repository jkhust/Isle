#include "Backdrop.h"

#include <OpenGL/gl.h>
#include "Game.h"
#include "../render/RenderManager.h"

Backdrop::Backdrop()
{
	// nothing yet
}

// ----------------------------------------------------------------
void Backdrop::init( Game* game )
{
	mGame = game;
	mVisible = true;
	clear();	
}

// ----------------------------------------------------------------
void Backdrop::clear()
{
	mStops.clear();
}

// ----------------------------------------------------------------
void Backdrop::add( float percent, const Vec3f& color )
{
    //find insert point
    int insertPos = 0;
    if( mStops.size() > 0 )
    {
	    while( ( insertPos < mStops.size() ) && mStops[insertPos].percent < percent ) {
	        ++insertPos;
	    }
    }

   	mStops.insert( mStops.begin() + insertPos, BackdropStop( percent, color ) );	
}

// -----------------------------------------------------------------
void Backdrop::render()
{
	// *** NOTE: make it so this class doesn't depend on opengl directly.
	glDepthMask( GL_FALSE );
	glDisable( GL_DEPTH_TEST );
	//glDepthFunc( GL_ALWAYS );

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f );

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
		glBegin( GL_QUADS );
		for( int i=0; i < mStops.size() - 1; ++i )
		{
			glColor3f( mStops[i].color.x, mStops[i].color.y, mStops[i].color.z );
			glVertex2f( 0.0f, mStops[i].percent );
			glVertex2f( 1.0f, mStops[i].percent );

			glColor3f(  mStops[i+1].color.x, mStops[i+1].color.y, mStops[i+1].color.z );
			glVertex2f( 1.0f, mStops[i+1].percent );
			glVertex2f( 0.0f, mStops[i+1].percent );
		}
		glEnd();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();

	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();

	glDepthMask( GL_TRUE );
	glEnable( GL_DEPTH_TEST );
	//glDepthFunc( GL_LEQUAL );
}