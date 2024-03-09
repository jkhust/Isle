#pragma once

#include "../util/Vec3f.h"

class Scene;

class Camera
{
private:
	Scene * mScene;
	int   	mType;

	float mYaw;     // 0 to 360 deg.
	float mPitch;   // 80 to -80 deg.

	Vec3f mPos;

	Vec3f mLookAt;   // basis
	Vec3f mRight;
	Vec3f mUp;

	float mZoom;

	void rebuildBasis();

public:
	static const int PERSPECTIVE = 1;
	static const int ORTHOGRAPHIC  = 2;

	Vec3f pos()    { return mPos; }
	Vec3f lookAt() { return mLookAt; }
	Vec3f up()     { return mUp; }
	float yaw()    { return mYaw; }
	float pitch()  { return mPitch; }
	float zoom()   { return mZoom; }

	Camera();
	void init( Scene* scene, int type );
	void reset();
	void moveTo( float x, float y, float z );
 	void moveBy( float front, float right, float up );
	void rotateTo( float yaw, float pitch );
 	void rotateBy( float yaw, float pitch );
 	void zoomTo( float zoom );
 	void zoomBy( float zoom );
	void apply();
};
