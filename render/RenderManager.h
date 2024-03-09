#pragma once

#include <stdlib.h>
#include <SDL.h>
#include <SDL_TTF.h>

#include <string>
#include "../util/Vec3f.h"

class Game;

class RenderManager
{
private:
	Game * mGame;
	SDL_Window  * mWindow;
	SDL_Surface * mScreen;
	SDL_GLContext mContext;

	TTF_Font 	* mRegularFont;

	int           mCullMode;
	bool          mCullingEnabled;
	bool          mWireframe;
	bool 		  mAntialiasingEnabled;

	Vec3f         mClearColor;

public:
 	const float SCREEN_WIDTH = 1152.0f;
	const float SCREEN_HEIGHT = 648.0f;

	Vec3f&        clearColor() { return mClearColor; }
	void setAntialiasingEnabled( bool value );

	RenderManager();

	void init( Game* game );
	void shutdown();

	Vec3f screenToScene( int x, int y );
	void renderBox( float x, float y, float z, float size, float r, float g, float b );
	void renderWireSphere( const Vec3f& pos, const Vec3f& color, float radius );
	void renderLine( const Vec3f& start, const Vec3f& startColor, const Vec3f& end, const Vec3f& endColor );
	void renderText( const std::string& text, int x, int y );
	void renderSolidQuad( const Vec3f& a, const Vec3f& b, const Vec3f& c, const Vec3f& d, const Vec3f& color, float alpha );
	void renderWireQuad( const Vec3f& a, const Vec3f& b, const Vec3f& c, const Vec3f& d, const Vec3f& color, float alpha, float lineWidth );

	void cycleCulling();
	void toggleWireframe();
	void toggleLighting();

	void setClearColor( float r, float g, float b );
	void setPerspective( float zoom = 1.0f );
	void setOrthographic( float zoom = 1.0f );
	void enableOrtho2D();   // for fonts, backdrops, UI which draw in screen coords
	void disableOrtho2D();
	void unprojectAll( int mouseX, int mouseY, Vec3f& scenePos, Vec3f& pickOrigin, Vec3f& pickDir);

	void beginFrame();
	void endFrame();
};

