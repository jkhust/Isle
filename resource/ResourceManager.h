#pragma once

#include <map>

class Game;
class TextureResource;

class ResourceManager
{
private:
	Game* mGame;
	std::map< int, TextureResource* > mTextures;

public:
	TextureResource * texture( int id ) { return mTextures[id]; }

	ResourceManager();

	void init( Game* game );
	void shutdown();

	void tick( double dt );
	void loadTexture( int id, const char * path );

};
