#include "Fader.h"

#include <OpenGL/gl.h>
#include "../util/GameMath.h"

Fader::Fader()
{
}

// ----------------------------------------------------------------
void Fader::init( Game* game )
{
	mGame = game;

	mFSM.init( this );
	mFSM.addState( IDLE_STATE, &Fader::whileIdle );
	mFSM.addState( FADING_STATE, &Fader::whileFading );
	mFSM.addTransition( IDLE_STATE, FADING_STATE );
	mFSM.addTransition( FADING_STATE, IDLE_STATE );
}

// ----------------------------------------------------------------
//void Fader::fade( const Vec3f& color, float startAlpha, float endAlpha, float duration, float stayDuration, EaseFunc easeFunc = Ease::lerp, GameFunction callbackFunc )
void Fader::fade( const Vec3f& color, float startAlpha, float endAlpha, float duration, float delay, float stayDuration, GameFunc callbackFunc )

{
	mColor.set( color );
	mStartAlpha = startAlpha;
	mEndAlpha = endAlpha;

	mDelay = delay;
	mDuration = duration;
	mStayDuration = stayDuration;

	//mEaseFunc = easeFunc;
	mCallbackFunc = callbackFunc;

	mFSM.transitionTo( FADING_STATE );
}

// ----------------------------------------------------------------
void Fader::tick( double dt )
{
	mFSM.tick( dt );
}

// ----------------------------------------------------------------
void Fader::whileIdle( double dt )
{
	// nothing yet
}

// ----------------------------------------------------------------
void Fader::whileFading( double dt  )
{
	float timeSoFar = mFSM.timeInState() - mDelay;

	float t = GameMath::clampf( timeSoFar, 0.0f, mDuration );
	mRenderAlpha = GameMath::lerp( mStartAlpha, mEndAlpha, t / mDuration );
	//mRenderAlpha = mEaseFunc( startAlpha, endAlpha, t, mDuration );

	if( timeSoFar >= ( mDuration + mStayDuration ) )
	{
		mFSM.transitionTo( IDLE_STATE );

		if( mCallbackFunc )
		{
			(mGame->*mCallbackFunc)();
		}
	}
}

// ----------------------------------------------------------------
void Fader::render()
{
	if( mRenderAlpha == 0.0f ) return;

	// *** NOTE: make it so this class doesn't depend on opengl directly.
	glEnable( GL_BLEND );

	glDepthMask( GL_FALSE );
	glDisable( GL_DEPTH_TEST );
	//glDepthFunc( GL_ALWAYS );

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f );

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
		glBegin( GL_QUADS );
		{
			glColor4f( mColor.x, mColor.y, mColor.z, mRenderAlpha );
			glVertex2f( 0.0f, 0.0f );
			glVertex2f( 1.0f, 0.0f );
			glVertex2f( 1.0f, 1.0f );
			glVertex2f( 0.0f, 1.0f );
		}
		glEnd();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();

	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();

	glDisable( GL_BLEND );
	glDepthMask( GL_TRUE );
	glEnable( GL_DEPTH_TEST );
	//glDepthFunc( GL_LEQUAL );
}

// ----------------------------------------------------------------
