#include "InputManager.h"

#include "../game/Game.h"
#include "../render/RenderManager.h"

#include <SDL.h>

// ----------------------------------------------------------------------------
InputManager::InputManager()
{
}

// ----------------------------------------------------------------------------
void InputManager::init( Game* game )
{
	mGame = game;
}

// ----------------------------------------------------------------------------
void InputManager::tick( double dt )
{
	mLastMouseX = mMouseX;
	mLastMouseY = mMouseY;
	SDL_GetMouseState( &mMouseX, &mMouseY );
	mMouseDX = mMouseX - mLastMouseX;
	mMouseDY = mMouseY - mLastMouseY;
    mGame->renderManager()->unprojectAll( mMouseX, mMouseY, mScenePos, mPickOrigin, mPickDir );

	// add all new events.
	mEvents.clear();

	SDL_Event e;
    while( SDL_PollEvent( &e ) != 0 )
    {
    	if( e.type == SDL_QUIT )
    	{
    		mQuit = true;
    	}
    	else if( e.type == SDL_MOUSEMOTION )
    	{
    		InputEvent ie;
    		ie.type = MOUSE_MOVE;
    		ie.mouseX = mMouseX;
    		ie.mouseY = mMouseY;
    		ie.keyCode = 0;
            ie.scenePos.set( mScenePos );

    	    mEvents.push_back( ie );
    	}
    	else if( e.type == SDL_MOUSEBUTTONDOWN )
    	{
    	    mLeftMouseHeld = true;

    	    InputEvent ie;
    		ie.type = LMOUSE_DOWN;
    		ie.mouseX = mMouseX;
    		ie.mouseY = mMouseY;
    		ie.keyCode = 0;
            ie.scenePos.set( mScenePos );
 
    	    mEvents.push_back( ie );
    	}
    	else if( e.type == SDL_MOUSEBUTTONUP )
    	{
    		mLeftMouseHeld = false;

 			InputEvent ie;
    		ie.type = LMOUSE_UP;
    		ie.mouseX = mMouseX;
    		ie.mouseY = mMouseY;
    		ie.keyCode = 0;
            ie.scenePos.set( mScenePos );
 
    	    mEvents.push_back( ie );
    	}
    	else if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    	{
    		mKeysHeld[ e.key.keysym.sym ] = true;

 			InputEvent ie;
    		ie.type = KEY_DOWN;
    		ie.mouseX = mMouseX;
    		ie.mouseY = mMouseY;
    		ie.keyCode = e.key.keysym.sym;
            ie.scenePos.set( mScenePos );
 
    		mEvents.push_back( ie );
    	}
    	else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    	{
    		mKeysHeld[ e.key.keysym.sym ] = false;

 			InputEvent ie;
    		ie.type = KEY_UP;
    		ie.mouseX = mMouseX;
    		ie.mouseY = mMouseY;
    		ie.keyCode = e.key.keysym.sym;
            ie.scenePos.set( mScenePos );
 
    		mEvents.push_back( ie );
    	}
    }
}

// ----------------------------------------------------------------------------
bool InputManager::keyHeld( int keyCode )
{ 
	return ( ( mKeysHeld.find( keyCode ) != mKeysHeld.end() ) && mKeysHeld[ keyCode ] == true );
}

// ----------------------------------------------------------------------------
bool InputManager::handle( InputEvent &event, int type, int keyCode )
{
	for(int i=0; i < mEvents.size(); ++i )
	{
		if( mEvents.at(i).type == type && mEvents.at(i).keyCode == keyCode )
		{
			event = mEvents.at( i );
			mEvents.erase( mEvents.begin() + i );
			return true;
		}
	}

	return false;
}

// ----------------------------------------------------------------------------
void InputManager::shutdown()
{
	// nothing yet
}

// ----------------------------------------------------------------------------
void InputManager::debug()
{
	//printf("Keys / Events : %d %d\n", mKeysHeld.size(), mEvents.size() );
}