#include "Console.h"
#include "Game.h"
#include "../input/InputManager.h"
#include "../util/GameMath.h"

Console::Console( Game * game )
{
	mGame = game;
}

// ---------------------------------------------------------------------
void Console::enter()
{
	mCommand = "";
	mLastCommand = mPastCommands.size();
}

// ---------------------------------------------------------------------
void Console::tick ( double dt )
{
	InputEvent e;
	if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_BACKSPACE ) )
	{
		mCommand = mCommand.substr(0, mCommand.length() - 1 );
	}
	else if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_UP ) )
	{
		if( mPastCommands.size() == 0 ) return;

		mLastCommand = GameMath::maxi( mLastCommand - 1, 0 );
		mCommand = mPastCommands[mLastCommand];
	}
	else if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_DOWN ) )
	{
		if( mPastCommands.size() == 0 ) return;

		mLastCommand = GameMath::mini( mLastCommand + 1, mPastCommands.size() );
		if( mLastCommand == mPastCommands.size() )
		{
			mCommand = "";
		}
		else
		{
			mCommand = mPastCommands[mLastCommand];
		}
	}
	// pressed a key for the buffer
	else
	{
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_a ) ) mCommand += "a"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_b ) ) mCommand += "b"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_c ) ) mCommand += "c"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_d ) ) mCommand += "d"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_e ) ) mCommand += "e"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_f ) ) mCommand += "f"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_g ) ) mCommand += "g"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_h ) ) mCommand += "h"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_i ) ) mCommand += "i"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_j ) ) mCommand += "j"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_k ) ) mCommand += "k"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_l ) ) mCommand += "l"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_m ) ) mCommand += "m"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_n ) ) mCommand += "n"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_o ) ) mCommand += "o"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_p ) ) mCommand += "p"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_q ) ) mCommand += "q"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_r ) ) mCommand += "r"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_s ) ) mCommand += "s"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_t ) ) mCommand += "t"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_u ) ) mCommand += "u"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_v ) ) mCommand += "v"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_w ) ) mCommand += "w"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_x ) ) mCommand += "x"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_y ) ) mCommand += "y"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_z ) ) mCommand += "z"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_EQUALS ) ) mCommand += "="; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_SPACE ) ) mCommand += " "; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_1 ) ) mCommand += "1"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_2 ) ) mCommand += "2"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_3 ) ) mCommand += "3"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_4 ) ) mCommand += "4"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_5 ) ) mCommand += "5"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_6 ) ) mCommand += "6"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_7 ) ) mCommand += "7"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_8 ) ) mCommand += "8"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_9 ) ) mCommand += "9"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_0 ) ) mCommand += "0"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_SLASH ) ) mCommand += "/"; 
		if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_PERIOD ) ) mCommand += "."; 
	}
}

// ---------------------------------------------------------------------
void Console::submitCommand()
{
	mPastCommands.push_back( mCommand );
	mGame->runCommand( mCommand );
	mCommand = "";
	mLastCommand = mPastCommands.size();
}