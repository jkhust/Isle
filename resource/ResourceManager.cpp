#include "ResourceManager.h"

#include "TextureResource.h"

// ----------------------------------------------------------------------------
ResourceManager::ResourceManager()
{
}

// ----------------------------------------------------------------------------
void ResourceManager::init( Game * game )
{
	mGame = game;
}

// ----------------------------------------------------------------------------
void ResourceManager::tick( double dt )
{
	// printf("Resources: %d\n", mTextures.size() );
}

// ----------------------------------------------------------------------------
void ResourceManager::loadTexture( int id, const char * path )
{ 
	mTextures[id] = new TextureResource( path );
}

// ----------------------------------------------------------------------------
void ResourceManager::shutdown()
{
	typedef std::map< int, TextureResource* >::iterator textureIterator;

	for(textureIterator it = mTextures.begin(); it != mTextures.end(); it++)
	{
		it->second->cleanup();
	}

	mTextures.clear();
}