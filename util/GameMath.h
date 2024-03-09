#pragma once

#include "Vec3f.h"
#include "Ray.h"

class GameMath
{
public:
	constexpr static const float PI      = 3.1415926535897932384626433832795f;
	constexpr static const float EPSILON = 0.0001f;

	static int maxi( int a, int b );
	static int mini( int a, int b );
	static float maxf( float a, float b);
	static float minf( float a, float b );
	static int randi( int min, int max );
	static float randf(float min, float max);
	static float lerp( float a, float b, float t );
	static bool isPointInTriangle( const Vec3f &a, const Vec3f &b, const Vec3f &c, float x, float y, float z );
	static Vec3f randomPointInTriangle( const Vec3f& a, const Vec3f& b, const Vec3f &c );
	static int   clampi( int value, int low, int high );
	static float clampf( float value, float low, float high );
	static float degToRad( float deg );

	// 10 sets of normals
	static float clampAngle( float angle );
	static bool pointInAABB( const Vec3f& p, const Vec3f& mins, const Vec3f& maxs );

	// ray tracing
	static bool rayInAABB( const Ray& r, const Vec3f& mins, const Vec3f& maxs, float& t );
	static bool rayInTriangle( const Ray& r, const Vec3f &a, const Vec3f& b, const Vec3f& c, float& out );
	static bool rayInPlane( const Ray& r, const Vec3f& p, const Vec3f& normal, Vec3f& hit );
};