#pragma once

#include "SDL2/SDL_mixer.h"

class SoundResource
{
private:
	Mix_Chunk *mChunk;

public:
	SoundResource( const char *path );

	void play();
	void cleanup();
};