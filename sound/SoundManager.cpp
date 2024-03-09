#include "SoundManager.h"

#include "SoundResource.h"
#include <stdlib.h>
#include <cassert>
#include <string>
#include <functional>

SoundManager::SoundManager()
{
	// nothing yet
}

// ----------------------------------------------------------------
void SoundManager::init( Game * game )
{
	mGame = game;

  	//Initialize SDL_mixer
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
    {
    	assert( !"Init audio failed." ); 
    }
}

// ----------------------------------------------------------------
void SoundManager::loadSound( const char *id, const char *path )
{
	std::size_t intId = std::hash<std::string>{}(id);

	SoundResource *res = new SoundResource( path );
	mSounds[intId] = res;

	printf("Loaded sound %s\n", path );
}

// ----------------------------------------------------------------
void SoundManager::playSound( const char *id )
{
	std::size_t intId = std::hash<std::string>{}(id);

    SoundResource *res = mSounds[intId];
    if( res )
    {
    	res->play();
    }
    else
    {
    	printf("WARNING: sound not found: %s\n", id );
    }
}

// ----------------------------------------------------------------
void SoundManager::shutdown()
{      
    for( std::map<int, SoundResource *>::iterator it = mSounds.begin() ; it != mSounds.end() ; ++it )
    {
    	SoundResource *res = it->second;

    	res->cleanup();
    	delete res;
    }
}