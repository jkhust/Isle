#pragma once

#include <string>

#include "../Scene.h"
#include "../../fsm/FSM.h"
#include "../../util/Vec3i.h"
#include "../../world/Patch.h"

class Game;
class Console;

class EditingScene : public Scene
{
private:

	FSM<EditingScene> 			mFSM;

	Console * 		  			mConsole;
	int                         mCurrMaterial;
	int 						mCurrLight;

	// edit state
	Patch * 					mPlanePatch;

	int 						mStopIndex;       // for editing backdrop stops
	float 						mStopMinPercent;
	float					    mStopMaxPercent;

	void whileEditing( double dt );
	void whileInConsole( double dt );
	void whileEditingBackdrop( double dt );

public:
	constexpr static const float CAMERA_MOVE_SPEED = 300.0f;  // units/sec
	constexpr static const float CAMERA_TURN_SPEED = 20.0f;  // deg/sec

	static const int EDIT_MODE = 1;
	static const int CONSOLE_MODE = 2;
	static const int BACKDROP_MODE = 3;

	static const int MAX_LIGHTS = 10;

	int currMaterial() { return mCurrMaterial; }
	int currLight() { return mCurrLight; }

	EditingScene( Game * game );

	void activate();

	void allowEnterConsole();

	void allowPause();
	void allowAdjustCamera( double dt );
	void allowCycleCurrentMaterial();
	void allowCycleCurrentLight();

	void allowAddBlocks();
	void allowAddRemovePatches();
	void allowUpdateMaterial();
	void allowEditLight();

	// backdrop editing
	void beginEditingBackdrop( int stop );

	void showBackdropDetails();
	void showEditDetails();

	void tick( double dt );
	void render();
};