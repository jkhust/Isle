#include "TextureResource.h"
#include <cassert>

TextureResource::TextureResource( const char * path )
{
	mPath = path;
	// load the surface
	mSurface = NULL;
	mSurface = IMG_Load( path );
	if( mSurface == NULL )
	{
		printf("Failed to load texture %s\n", path );
		assert( false );
	}

	// create an OpenGL texture for this surface
	// http://www.sdltutorials.com/sdl-tip-sdl-surface-to-opengl-texture
	glGenTextures( 1, &mTextureId );
	glBindTexture( GL_TEXTURE_2D, mTextureId );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	int format = ( mSurface->format->BytesPerPixel == 4 ) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, format, mSurface->w, mSurface->h, 0, format, GL_UNSIGNED_BYTE, mSurface->pixels);
	printf("Loaded texture %s as %d w %d h %d bpp %d\n", path, mTextureId, mSurface->w, mSurface->h, mSurface->format->BytesPerPixel );
}

// -----------------------------------------------------------------
void TextureResource::cleanup()
{
	//printf("Freeing texture %d\n", mTextureId );
	glDeleteTextures( 1, &mTextureId );
	SDL_FreeSurface( mSurface );
	mSurface = NULL;
}