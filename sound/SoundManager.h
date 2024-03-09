#pragma once

#include <map>

class Game;
class SoundResource;

class SoundManager
{
private:
	Game * mGame;
	std::map<int, SoundResource *> mSounds;
	//std::map<int, MusicResource *> mMusic;

public:
	SoundManager();

	void init( Game* game);
	void shutdown();

	void loadSound( const char *id, const char *path );
	void playSound( const char *id );
	///void startMusic( const char *id );
	//void stopMusic( const char *id );
};