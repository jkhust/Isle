#include "Camera.h"

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include "../util/GameMath.h"
#include "../util/Vec3f.h"
#include "../scene/Scene.h"
#include "../game/Game.h"
#include "../render/RenderManager.h"

// ------------------------------------------------------------
void Camera::rebuildBasis()
{
	// find front vector
	mLookAt.fromAngles( mYaw, mPitch );
	mUp.fromAngles( mYaw, mPitch + 90.0f );
	mRight = Vec3f::cross( mLookAt, mUp );
}

// ------------------------------------------------------------
Camera::Camera()
{
	mType = Camera::PERSPECTIVE;
	reset();
}

// ------------------------------------------------------------
void Camera::reset()
{
	moveTo( 0.0f, 815.0f, 0.0f );

	mYaw = 0.0f;
	mPitch = 0.0f;
	rotateBy( 45.0f, -35.0f );

	mZoom = 1.0f;
}

// ------------------------------------------------------------
void Camera::init( Scene *scene, int type )
{
	mScene = scene;
	mType = type;
}

// ------------------------------------------------------------
void Camera::moveTo( float x, float y, float z )
{
	mPos.set( x, y, z );
}

// ------------------------------------------------------------
void Camera::moveBy( float front, float right, float up )
{
	mPos.add( mLookAt.x * front, mLookAt.y * front, mLookAt.z * front );
	mPos.add( mRight.x * right, mRight.y * right, mRight.z * right );
	mPos.add( mUp.x * up, mUp.y * up, mUp.z * up );
}

// ------------------------------------------------------------
void Camera::rotateTo( float yaw, float pitch )
{
	mYaw = yaw;
	mPitch = pitch;
	rebuildBasis();
}

// ------------------------------------------------------------
void Camera::rotateBy( float yaw, float pitch )
{
	mYaw = GameMath::clampAngle( mYaw + yaw );
	mPitch = GameMath::clampf( mPitch + pitch, -90.0f, 90.0f );
	rebuildBasis();
}

// ------------------------------------------------------------
void Camera::zoomTo( float zoom )
{
	mZoom = zoom;
}

// ------------------------------------------------------------
 void Camera::zoomBy( float zoom )
 {
 	mZoom += zoom;
 }

// ------------------------------------------------------------
void Camera::apply()
{
	// printf("camera %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f\n", mPos.x, mPos.y, mPos.z, 
	// 																			      mLookAt.x, mLookAt.y, mLookAt.z, 
	// 																			      mUp.x, mUp.y, mUp.z );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt( mPos.x, mPos.y, mPos.z, 
			   mPos.x + (mLookAt.x * 9999.0f), mPos.y + (mLookAt.y * 9999.0f), mPos.z + (mLookAt.z * 9999.0f), 
			   mUp.x, mUp.y, mUp.z );

	if( mType == Camera::PERSPECTIVE )
	{
		mScene->game()->renderManager()->setPerspective( mZoom );
	}
	else if( mType == Camera::ORTHOGRAPHIC )
	{
		mScene->game()->renderManager()->setOrthographic( mZoom );
	}
}