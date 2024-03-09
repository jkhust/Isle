#include "GameMath.h"

#include <cstdlib>
#include <stdio.h>
#include <math.h>

// -------------------------------------------------------------
int GameMath::maxi( int a, int b )
{
    if( a > b ) return a;
    return b;
}

// -------------------------------------------------------------
int GameMath::mini( int a, int b )
{
    if( a < b ) return a;
    return b;
}

// -------------------------------------------------------------
float GameMath::maxf( float a, float b )
{
    if( a > b ) return a;
    return b;
}

// -------------------------------------------------------------
float GameMath::minf( float a, float b )
{
    if( a < b ) return a;
    return b;
}

// -------------------------------------------------------------
int GameMath::randi( int min, int max )
{
	return rand() % (max - min + 1 ) + min;
}

// -------------------------------------------------------------
float GameMath::randf(float min, float max)
{
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = max - min;
    float r = random * diff;
	return min + r;
}

// -------------------------------------------------------------
float GameMath::lerp( float a, float b, float t )
{
	return( a + ( b - a ) * t );
}

// -------------------------------------------------------------
float GameMath::degToRad( float deg )
{
    return deg * PI / 180.0f;
}

// -------------------------------------------------------------
bool GameMath::isPointInTriangle( const Vec3f &u, const Vec3f &v, const Vec3f &w, float x, float y, float z )
{
	double denom = ((v.y - w.y) * (u.x - w.x)) + ((w.x - v.x) * (u.y - w.y));
	double a = ((v.y - w.y) * (x - w.x) + (w.x - v.x) * (y - w.y)) / denom;
	double b = ((w.y - u.y) * (x - w.x) + (u.x - w.x) * (y - w.y)) / denom;
	double c = 1 - a - b;

	return ((0 <= a && a <= 1) && (0 <= b && b <= 1) && (0 <= c && c <= 1));
}

// -------------------------------------------------------------
// https://www.opengl.org/discussion_boards/showthread.php/159436-Random-point-from-triangle
// TODO: find a more uniform method.
Vec3f GameMath::randomPointInTriangle( const Vec3f& a, const Vec3f& b, const Vec3f &c )
{
    float t1 = randf( 0, 100.0f );
    float t2 = randf( 0, 100.0f );
    float t3 = randf( 0, 100.0f );
    float sum = t1 + t2 + t3;

    t1 /= sum;
    t2 /= sum;
    t3 /= sum;

    Vec3f ret;
    ret.add( a.times( t1 ) );
    ret.add( b.times( t2 ) );
    ret.add( c.times( t3 ) );
    return ret;
}

// -------------------------------------------------------------
int GameMath::clampi( int value, int low, int high )
{
    return mini( maxi( value, low ), high );
}

// -------------------------------------------------------------
float GameMath::clampf( float value, float low, float high )
{
	return minf( maxf( value, low ), high );
}

// -------------------------------------------------------------
float GameMath::clampAngle( float angle )
{
    float ret = angle;

    while( ret > 360.0f )
        ret -= 360.0f;

    while( ret < 0.0f )
        ret += 360.0f;

    return ret;
}

// -------------------------------------------------------------
bool GameMath::pointInAABB( const Vec3f& p, const Vec3f& mins, const Vec3f& maxs )
{
    return( p.x >= mins.x && p.x <= maxs.x && p.y >= mins.y && p.y <= maxs.y && p.z >= mins.z && p.z <= maxs.z );
}

// -------------------------------------------------------------
// slab test
bool GameMath::rayInAABB( const Ray& r, const Vec3f& mins, const Vec3f& maxs, float& t )
{
    Vec3f c = Vec3f::center2( mins, maxs );
    Vec3f e = maxs.minus( c );
    Vec3f m = Vec3f::center2( r.start, r.end );
    Vec3f d = r.end.minus( m );
    m.sub( c );
    // Box center-point
    // Box halflength extents
    // Segment midpoint
    // Segment halflength vector
    // Translate box and segment to origin
    // Try world coordinate axes as separating axes
    float adx = fabs(d.x);
    if (fabs(m.x) > e.x + adx) return false;
    float ady = fabs(d.y);
    if (fabs(m.y) > e.y + ady ) return false;
    float adz = fabs(d.z);
    if (fabs(m.z) > e.z + adz) return false;
    // Add in an epsilon term to counteract arithmetic errors when segment is // (near) parallel to a coordinate axis (see text for detail)
    adx += EPSILON; ady += EPSILON; adz += EPSILON;
    // Try cross products of segment direction vector with coordinate axes
    if (fabs(m.y * d.z - m.z * d.y) > e.y * adz + e.z * ady) return false; 
    if (fabs(m.z * d.x - m.x * d.z) > e.x * adz + e.z * adx) return false; 
    if (fabs(m.x * d.y - m.y * d.x) > e.x * ady + e.y * adx) return false; 
    // No separating axis found; segment must be overlapping AABB return 1;
    return true;

/*
    double t1, t2, tmin, tmax;

    t1 = ( mins.x - r.start.x ) * r.dir_inv.x;
    t2 = ( maxs.x - r.start.x ) * r.dir_inv.x;
 
    tmin = GameMath::minf(t1, t2);
    tmax = GameMath::maxf(t1, t2);

    t1 = (mins.y - r.start.y ) * r.dir_inv.y;
    t2 = (maxs.y - r.start.y ) * r.dir_inv.y;
 
    tmin = GameMath::maxf(tmin, GameMath::minf(t1, t2));
    tmax = GameMath::minf(tmax, GameMath::maxf(t1, t2));

    t1 = ( mins.z - r.start.z ) * r.dir_inv.z;
    t2 = ( maxs.z - r.start.z ) * r.dir_inv.z;
 
    tmin = GameMath::maxf(tmin, GameMath::minf(t1, t2));
    tmax = GameMath::minf(tmax, GameMath::maxf(t1, t2));
 
    // choose proper t depending of ray began in box or not.
    t = (tmin < 0 ? tmax : tmin );
    bool ret = tmax > GameMath::maxf(tmin, 0.0f);

    return ret;
    */
}

// -------------------------------------------------------------
// Möller–Trumbore intersection algorithm
bool GameMath::rayInTriangle( const Ray& r, const Vec3f &a, const Vec3f& b, const Vec3f& c, float& out )
{
    //printf("Ray in Triangle...\n");

    //Find vectors for two edges sharing V1
    Vec3f ab( b.minus( a ) );
    Vec3f ac( c.minus( a ) );
    //Begin calculating determinant - also used to calculate u parameter
    Vec3f P = Vec3f::cross( r.dir, ac );

    //if determinant is near zero, ray lies in plane of triangle or ray is parallel to plane of triangle
    float det = Vec3f::dot(ab, P);
    //NOT CULLING
    if( det > -EPSILON && det < EPSILON) return false;  // TODO: understand this.
    float inv_det = 1.0f / det;

    //calculate distance from V1 to ray origin
    Vec3f T = r.start.minus( a );

    //Calculate u parameter and test bound
    float u = Vec3f::dot(T, P) * inv_det;
    //The intersection lies outside of the triangle
    if(u < 0.0f || u > 1.0f) return false;

    //Prepare to test v parameter
    Vec3f Q = Vec3f::cross( T, ab );

    //Calculate V parameter and test bound
    float v = Vec3f::dot( r.dir, Q ) * inv_det;

    //The intersection lies outside of the triangle
    if(v < 0.0f || u + v  > 1.0f) return false;

    float t = Vec3f::dot( ac, Q ) * inv_det;

    if( t > EPSILON ) { //ray intersection
        out = t;
        return true;
    }

    return false;
}

// -------------------------------------------------------------
// standard ray-in-plane test, used for placing patches along a plane in edit mode.
bool GameMath::rayInPlane( const Ray& r, const Vec3f& p, const Vec3f& normal, Vec3f& hit )
{ 
    float denom = Vec3f::dot( normal, r.dir );
    if( fabs(denom) > EPSILON ) // your favorite epsilon
    {
        float t = Vec3f::dot( p.minus( r.start ), normal ) / denom;
        if (t >= 0)
        {
            hit.set( r.start.plus( r.dir.times( t ) ) );
            return true; // you might want to allow an epsilon here too
        }
    }
    return false;
} 
