#pragma once

#include "Camera.h"

class Game;

class Scene
{
protected:
	Game  *mGame;
	Camera mCamera;
	bool   mActive;
	bool   mPlaying; // (ticking)

public:
	Game   * game()	   { return mGame; }
	Camera & camera()  { return mCamera; }
	bool     isActive() { return mActive; }
	bool     isPlaying() { return mPlaying; }

	Scene( Game * game );

	virtual void activate();
	void deactivate();
	void pause();
	void resume();

	virtual void tick( double dt ) = 0;
	virtual void render() = 0;
};