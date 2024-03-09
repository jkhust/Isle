#pragma once

#include <string>
#include <SDL.h>
#include <SDL_image.h> 
#include <OpenGL/gl.h>

class TextureResource
{
private:
	std::string   mPath;
	SDL_Surface * mSurface;
	GLuint        mTextureId;

public:
	GLuint textureId() { return mTextureId; }

	TextureResource( const char * path );

	void cleanup();
};