#pragma once

#include <SDL.h>
#include <vector>
#include <map>

#include "../util/Vec3f.h"

class Game;

class InputEvent
{
public:
	int type;
	int mouseX;
	int mouseY;
	int keyCode;
	Vec3f scenePos;
};

// ------------------------------------------------------------------------
class InputManager
{
private:
	Game* mGame = NULL;

	std::vector<InputEvent> mEvents;
	std::map<int, bool>     mKeysHeld;
	bool                    mLeftMouseHeld = false;
	bool                    mQuit = false;

	int mMouseX = 0;
	int mMouseY = 0;
	int mLastMouseX = 0;
	int mLastMouseY = 0;
	int mMouseDX = 0;
	int mMouseDY = 0;
	Vec3f mScenePos;
	Vec3f mPickOrigin;
	Vec3f mPickDir;

public:
	static const int LMOUSE_DOWN = 1;
	static const int LMOUSE_UP = 2;
	static const int MOUSE_MOVE = 3;
	static const int KEY_DOWN = 4;
	static const int KEY_UP = 5;

	bool leftMouseHeld() { return mLeftMouseHeld; }
	bool mouseMoved() { return (mMouseDX != 0 || mMouseDY != 0); }
	bool quit() { return mQuit; }

	int mouseX() { return mMouseX; }
	int mouseY() { return mMouseY; }
	int lastMouseX() { return mLastMouseX; }
	int lastMouseY() { return mLastMouseY; }
	int mouseDX() { return mMouseDX; }
	int mouseDY() { return mMouseDY; }
	const Vec3f& scenePos() { return mScenePos; }
	const Vec3f& pickOrigin() { return mPickOrigin; }
	const Vec3f& pickDir() { return mPickDir; }

	// ----
	InputManager();

	void init( Game* game );
	void shutdown();

	void tick( double dt );
	bool keyHeld( int keyCode );
	bool handle( InputEvent &event, int type, int keyCode = 0 );
	void debug();
};