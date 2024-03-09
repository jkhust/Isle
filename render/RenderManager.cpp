#include "RenderManager.h"

#include <stdlib.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>

#include <string>
#include <cassert>

RenderManager::RenderManager()
{
	mGame = NULL;
	mScreen = NULL;
	mWindow = NULL;
}

// -----------------------------------------------------------------
void RenderManager::init( Game* game )
{
	mGame = game;

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    mWindow = SDL_CreateWindow(
		        "Isle",                            // window title
		        SDL_WINDOWPOS_UNDEFINED,           // initial x position
		        SDL_WINDOWPOS_UNDEFINED,           // initial y position
		        SCREEN_WIDTH,                      // width, in pixels
		        SCREEN_HEIGHT,                     // height, in pixels
		        SDL_WINDOW_OPENGL                  // flags - see below
		    );

    if ( mWindow == NULL ) {
        printf("Could not create window: %s\n", SDL_GetError());
        assert( false );
    }

		mScreen = SDL_GetWindowSurface( mWindow );

		mContext = SDL_GL_CreateContext( mWindow );
		assert( mContext != NULL );

		//Use Vsync
    if( SDL_GL_SetSwapInterval( 1 ) < 0 )
    {
    	assert(!"Failed to set vsync");
    }

    // *** fonts ****
    if( TTF_Init() < 0 )
    {
        printf("Could not load font library: %s\n", TTF_GetError());
    }

    mRegularFont = NULL;
	mRegularFont = TTF_OpenFont( "assets/fonts/Abel-Regular.ttf", 18 );
	assert( mRegularFont != NULL );

	glEnable(GL_DEPTH_TEST);

	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	mCullMode = GL_BACK;
	mCullingEnabled = true;
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);  

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);  // needed for fader and font stuff.
	glDisable(GL_BLEND);

	mWireframe = false;

	mAntialiasingEnabled = true;
	glEnable( GL_MULTISAMPLE );

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
}

// -----------------------------------------------------------------
void RenderManager::shutdown()
{
	TTF_CloseFont( mRegularFont );
	
    SDL_FreeSurface( mScreen );
    SDL_DestroyWindow( mWindow );
}

// -----------------------------------------------------------------
void RenderManager::setOrthographic( float zoom )
{
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
    glOrtho( -SCREEN_WIDTH / 2.0f * zoom, SCREEN_WIDTH / 2.0f * zoom, -SCREEN_HEIGHT / 2.0f * zoom, SCREEN_HEIGHT / 2.0f * zoom, 0.1f, 5000.0f );
}

// -----------------------------------------------------------------
// zoom unused for now.
void RenderManager::setPerspective( float zoom )
{
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
    gluPerspective( 45.0f, SCREEN_WIDTH / SCREEN_HEIGHT, 0.1, 5000.0f );
}

// -----------------------------------------------------------------
void RenderManager::setClearColor( float r, float g, float b )
{
	mClearColor.set( r, g, b );
	glClearColor( r, g, b, 0.0f );  
}

// -----------------------------------------------------------------
void RenderManager::setAntialiasingEnabled( bool value )
{
	mAntialiasingEnabled = value;
	if( value )
	{
		glEnable( GL_MULTISAMPLE );
	}
	else
	{
		glDisable( GL_MULTISAMPLE );
	}
}

// -----------------------------------------------------------------
int roundDouble(double x)
{
	return (int)(x + 0.5);
}

// -----------------------------------------------------------------
int nextpoweroftwo(int x)
{
	double logbase2 = log(x) / log(2);
	return roundDouble(pow(2,ceil(logbase2)));
}

// -----------------------------------------------------------------
void RenderManager::enableOrtho2D()
{
	int vPort[4];
  
	glGetIntegerv(GL_VIEWPORT, vPort);
  
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, vPort[2], 0, vPort[3], -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

// -----------------------------------------------------------------
void RenderManager::disableOrtho2D()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();   

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();	
}

// -----------------------------------------------------------------
// https://www.gamedev.net/topic/284259-for-reference-using-sdl_ttf-with-opengl/
void RenderManager::renderText( const std::string& text, int x, int y )
{	
	enableOrtho2D();
	glEnable(GL_BLEND);
	glDisable( GL_DEPTH_TEST );
//	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );  apparently this isn't needed?

	// render text to an ARGB surface.
	SDL_Color color = { 255, 255, 0 };
	SDL_Surface *initial = TTF_RenderText_Solid(mRegularFont, text.c_str(), color);	

	// convert to power of two RGBA texture
	int w = nextpoweroftwo(initial->w);
	int h = nextpoweroftwo(initial->h);
	SDL_Surface *intermediary = SDL_CreateRGBSurface(0, w, h, 32, 
								0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 );  // lowest byte is red mask
	SDL_BlitSurface(initial, 0, intermediary, 0);
	
	/* Tell GL about our new texture */
	GLuint texId = 0;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, intermediary->pixels );    

	/* prepare to render our texture */
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f );
	
	/* Draw a quad at location */
	y = SCREEN_HEIGHT - y - h;
	glBegin(GL_QUADS);
		/* Recall that the origin is in the lower-left corner
		   That is why the TexCoords specify different corners
		   than the Vertex coors seem to. */
		glTexCoord2f(0.0f, 1.0f); 
			glVertex3f(x , y, 0.01);
		glTexCoord2f(0.0f, 0.0f); 
			glVertex3f(x , y + h, 0.01);
		glTexCoord2f(1.0f, 0.0f); 
			glVertex3f(x + w, y + h, 0.01);
		glTexCoord2f(1.0f, 1.0f); 
			glVertex3f(x + w, y, 0.01);
	glEnd();
	
	/* Bad things happen if we delete the texture before it finishes */
	glFinish();
	
	/* Clean up */
	SDL_FreeSurface(initial);
	SDL_FreeSurface(intermediary);
	glDeleteTextures(1, &texId);

	glEnable( GL_DEPTH_TEST );
	glDisable(GL_BLEND);
	disableOrtho2D();
}

// -----------------------------------------------------------------
void RenderManager::beginFrame( void )
{
	glMatrixMode( GL_MODELVIEW );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

// -----------------------------------------------------------------
void RenderManager::renderBox( float x, float y, float z, float size, float r, float g, float b )
{
	glPushMatrix();

	glTranslatef( x, y, z );
	glScalef( size / 2.0f, size / 2.0f, size / 2.0f);
    glColor3f( r, g, b );

	glBegin( GL_LINES );
		// top lines
		glVertex3f( -1.0f, -1.0f, 1.0f );
		glVertex3f( -1.0f, 1.0f, 1.0f );

		glVertex3f( -1.0f, 1.0f, 1.0f );
		glVertex3f( 1.0f, 1.0f, 1.0f );

		glVertex3f( 1.0f, 1.0f, 1.0f );
		glVertex3f( 1.0f, -1.0f, 1.0f );

		glVertex3f( 1.0f, -1.0f, 1.0f );
		glVertex3f( -1.0f, -1.0f, 1.0f );

		// sides
		glVertex3f( -1.0f, -1.0f, -1.0f );
		glVertex3f( -1.0f, -1.0f, 1.0f );

		glVertex3f( -1.0f, 1.0f, -1.0f );
		glVertex3f( -1.0f, 1.0f, 1.0f );

		glVertex3f( 1.0f, 1.0f, -1.0f );
		glVertex3f( 1.0f, 1.0f, 1.0f );

		glVertex3f( 1.0f, -1.0f, -1.0f );
		glVertex3f( 1.0f, -1.0f, 1.0f );

		// bottom
		glVertex3f( -1.0f, -1.0f, -1.0f );
		glVertex3f( -1.0f, 1.0f, -1.0f );

		glVertex3f( -1.0f, 1.0f, -1.0f );
		glVertex3f( 1.0f, 1.0f, -1.0f );

		glVertex3f( 1.0f, 1.0f, -1.0f );
		glVertex3f( 1.0f, -1.0f, -1.0f );

		glVertex3f( 1.0f, -1.0f, -1.0f );
		glVertex3f( -1.0f, -1.0f, -1.0f );

	glEnd();

    glPopMatrix();
}

// -----------------------------------------------------------------
void RenderManager::renderWireSphere( const Vec3f& pos, const Vec3f& color, float radius )
{
	glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    	glTranslatef( pos.x, pos.y, pos.z );
    	glColor3f( color.x, color.y, color.z );
		glutWireSphere( radius, 16, 16 );
	glPopMatrix();
}

// -----------------------------------------------------------------
void RenderManager::renderLine( const Vec3f& start, const Vec3f& startColor, const Vec3f& end, const Vec3f& endColor )
{
	glBegin( GL_LINES );
		glColor3f( startColor.x, startColor.y, startColor.z );
		glVertex3f( start.x, start.y, start.z );
		glColor3f( endColor.x, endColor.y, endColor.z );
		glVertex3f( end.x, end.y, end.z );
	glEnd();
}

// -----------------------------------------------------------------
void RenderManager::renderSolidQuad( const Vec3f& a, const Vec3f& b, const Vec3f& c, const Vec3f& d,
									 const Vec3f& color, float alpha )
{
	glEnable( GL_BLEND );
	glColor4f( color.x, color.y, color.z, alpha );

	glBegin( GL_QUADS );
		glVertex3f( a.x, a.y, a.z );
		glVertex3f( b.x, b.y, b.z );
		glVertex3f( c.x, c.y, c.z );
		glVertex3f( d.x, d.y, d.z );
	glEnd();
	glDisable( GL_BLEND );
}

// -----------------------------------------------------------------
void RenderManager::renderWireQuad( const Vec3f& a, const Vec3f& b, const Vec3f& c, const Vec3f& d,
									const Vec3f& color, float alpha, float lineWidth )
{
	glEnable( GL_BLEND );
	glColor4f( color.x, color.y, color.z, alpha );
	glLineWidth( lineWidth );

	glBegin( GL_LINE_LOOP );
		glVertex3f( a.x, a.y, a.z );
		glVertex3f( b.x, b.y, b.z );
		glVertex3f( c.x, c.y, c.z );
		glVertex3f( d.x, d.y, d.z );
	glEnd();
	glDisable( GL_BLEND );
}

// -----------------------------------------------------------------
void RenderManager::cycleCulling()
{
	if( mCullMode == GL_BACK )
	{
		mCullMode = GL_FRONT;
		glCullFace( GL_FRONT );
		printf("Culling front.\n");
	}
	else if( mCullMode == GL_FRONT )
	{
		mCullMode = GL_FRONT_AND_BACK;
		glCullFace( GL_FRONT_AND_BACK );
		printf("Culling all.\n");
	}
	else if( mCullMode == GL_FRONT_AND_BACK && mCullingEnabled )
	{
		mCullingEnabled = false;
		glDisable( GL_CULL_FACE );
		printf("Culling none.\n");
	}
	else if( mCullMode == GL_FRONT_AND_BACK && !mCullingEnabled )
	{
		mCullingEnabled = true;
		mCullMode = GL_BACK;
		glEnable( GL_CULL_FACE );
		glCullFace( GL_BACK );
		printf("Culling back.\n");
	}
}

// -----------------------------------------------------------------
void RenderManager::toggleWireframe()
{
	mWireframe = !mWireframe;
	if( mWireframe)
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		printf("Switched to wireframe.\n");
	}
	else
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		printf("Switched to solid.\n");
	}
}

// -----------------------------------------------------------------
void RenderManager::endFrame()
{
	SDL_GL_SwapWindow( mWindow );	
}

// -----------------------------------------------------------------
void RenderManager::unprojectAll( int mouseX, int mouseY, Vec3f& scenePos, Vec3f& pickOrigin, Vec3f& pickDir )
{
	GLint viewport[4];
	GLdouble modelMatrix[16];
	GLdouble projectionMatrix[16];

	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);

	GLfloat winY = GLfloat(viewport[3] - mouseY);
	GLfloat winZ;
    glReadPixels( mouseX, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

	double x, y, z;
    gluUnProject( (double) mouseX, winY, winZ, 
    			   modelMatrix, projectionMatrix, viewport,
    			   &x, &y, &z );
    scenePos.set( x, y, z );

	gluUnProject( (double) mouseX, winY, 0.0f,              // Near Plane
	              modelMatrix, projectionMatrix, viewport,
	              &x, &y, &z );
	pickOrigin.set( x, y, z );

	gluUnProject( (double) mouseX, winY, 1.0f,              // Far Plane
	              modelMatrix, projectionMatrix, viewport,
	              &x, &y, &z );
	pickDir.set( x, y, z );
}

// -----------------------------------------------------------------
Vec3f RenderManager::screenToScene(int x, int y )
{
 	return Vec3f( 0.0f, 0.0f, 0.0f );
   //  GLfloat winX, winY, winZ;
   //  winX = (float)x;
   //  winY = (float)viewport[3] - (float)y;

   //  GLdouble posX, posY, posZ;

   // // printf("unprojected %0.2f %0.2f %0.2f to %0.2f %0.2f %0.2f\n", winX, winY, winZ, posX, posY, posZ );
   //  return Vec3f(posX, posY, posZ);
}