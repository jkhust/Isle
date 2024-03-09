#include "PlayingScene.h"

#include "../../game/Game.h"
#include "../../map/Map.h"
#include "../../world/World.h"
#include "../../input/InputManager.h"
#include "../../render/RenderManager.h"

PlayingScene::PlayingScene( Game * game ) : Scene( game )
{
	mCamera.init( this, Camera::ORTHOGRAPHIC );
}

// ----------------------------------------------------------------------------
void PlayingScene::activate()
{
	Scene::activate();
	mGame->renderManager()->setClearColor( 1.0f, 1.0f, 1.0f );
	mCamera.apply();

	mPlayState.init( mGame );
}

// ----------------------------------------------------------------------------
void PlayingScene::allowAdjustCamera( double dt )
{
	bool updated = false;

	// *** reset camera? ***
	InputEvent e;
	if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_r ) )
	{
		mCamera.reset();
		updated = true;
	}
	else
	{
		// *** rotate camera? ***
		// float yaw = 0.0f;
		// float pitch = 0.0f;
		// if( mGame->inputManager()->keyHeld( SDLK_LSHIFT ))
		// {
		// 	yaw = mGame->inputManager()->mouseDX() * CAMERA_TURN_SPEED * dt;
		// 	pitch = -1.0f * mGame->inputManager()->mouseDY() * CAMERA_TURN_SPEED * dt;
		// 	mCamera.rotateBy( yaw, pitch );	
		// 	updated |= (yaw != 0.0f || pitch != 0.0f );
		// }

		// **** move camera? ***
		float moveFront = 0.0f;
		float moveSide = 0.0f;
		float moveUp = 0.0f;
		float zoomAdjust = 0.0f;

		if( mGame->inputManager()->keyHeld( SDLK_w ) )
		{
			moveUp += 1.0f;
		}
		if( mGame->inputManager()->keyHeld( SDLK_s ) )
		{
			moveUp -= 1.0f;
		}
		if( mGame->inputManager()->keyHeld( SDLK_a ) )
		{
			moveSide -= 1.0f;
		}
		if( mGame->inputManager()->keyHeld( SDLK_d ) )
		{
			moveSide += 1.0f;
		}
		if( mGame->inputManager()->keyHeld( SDLK_q ) )
		{
			zoomAdjust -= 0.05f;
		}
		if( mGame->inputManager()->keyHeld( SDLK_e ) )
		{
			zoomAdjust += 0.05f;
		}


		mCamera.moveBy( moveFront * CAMERA_MOVE_SPEED * dt, 
						moveSide * CAMERA_MOVE_SPEED * dt,
						moveUp * CAMERA_MOVE_SPEED * dt );
		mCamera.zoomBy( zoomAdjust );
		updated |= (moveFront != 0.0f || moveSide != 0.0f || moveUp != 0.0f || zoomAdjust != 0.0f );
	}

	if( updated )
	{
		mCamera.apply();
	}
}

// ----------------------------------------------------------------------------
void PlayingScene::tick( double dt )
{
	allowAdjustCamera( dt );
	allowInteract( dt );
}

// ----------------------------------------------------------------------------
void PlayingScene::allowInteract( double dt )
{
	InputEvent e;
	if( mGame->inputManager()->handle( e, InputManager::LMOUSE_DOWN ) )
	{
		Patch *p = mGame->world()->getPatchAt( e.scenePos );
		if( p )
		{
			mPlayState.interact( p );
		}
	}
}

// ----------------------------------------------------------------------------
void PlayingScene::render()
{
	mPlayState.render();
}	
