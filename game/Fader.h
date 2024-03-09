#pragma once

#include "../fsm/FSM.h"
#include "../util/Vec3f.h"

class Game;

//typedef float ( Easer::*EaseFunc ) ( float, float, float, float );
typedef void  ( Game::*GameFunc ) ( void );

class Fader
{
private:
	Game *     mGame;
	FSM<Fader> mFSM;

	Vec3f mColor;
	float mStartAlpha;
	float mEndAlpha;

	float mDelay;
	float mDuration;
	float mStayDuration;

	//EaseFunc     mEaseFunc;
	GameFunc mCallbackFunc;

	// instance data
	float mRenderAlpha;

	void whileIdle( double dt );
	void whileFading( double dt );

public:	
	static const int IDLE_STATE = 0;
	static const int FADING_STATE = 1;

	Fader();
	void init( Game* game );

	// does fade and calls func when complete.
//	void fade( const Vec3f& color, float startAlpha, float endAlpha, float duration, float stayDuration, EaseFunc easeFunc = Ease::lerp, GameFunc callbackFunc = NULL );
	void fade( const Vec3f& color, float startAlpha, float endAlpha, float duration, float delay = 0.0f, float stayDuration = 0.0f, GameFunc callbackFunc = NULL );

	void tick( double dt );
	void render();
};

