#pragma once

#include "../Scene.h"
#include "PlayState.h"

class Game;

class PlayingScene : public Scene
{
private:
	PlayState mPlayState;

public:
	constexpr static const float CAMERA_MOVE_SPEED = 300.0f;  // units/sec
//	constexpr static const float CAMERA_TURN_SPEED = 20.0f;  // deg/sec

	PlayingScene( Game * game );

	void activate();

	void allowAdjustCamera( double dt );
	void allowInteract( double dt );

	void tick( double dt );
	void render();
};