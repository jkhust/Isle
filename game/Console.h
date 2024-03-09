#pragma once

#include <string>
#include <vector>

class Game;

class Console
{
private:
	Game *mGame;

	std::string				    mCommand;  	 
	std::vector<std::string>    mPastCommands;
	int                         mLastCommand;

public:
	Console( Game * game );

	const std::string& command() { return mCommand; }

	void enter();              // set stuff up.
	void tick ( double dt );
	void submitCommand();
};


