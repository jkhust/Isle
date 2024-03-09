#include "SoundResource.h"

#include <cassert>

// ----------------------------------------------------------------
SoundResource::SoundResource( const char * path )
{
	mChunk = NULL;
	mChunk = Mix_LoadWAV( path );
	assert( mChunk );
}

// ----------------------------------------------------------------
void SoundResource::play()
{
	Mix_PlayChannel( -1, mChunk, 0 );
}

// ----------------------------------------------------------------
void SoundResource::cleanup()
{
	Mix_FreeChunk( mChunk );
	mChunk = NULL;
}