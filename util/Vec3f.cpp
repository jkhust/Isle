#include "Vec3f.h"

#include "GameMath.h"

#include <iostream>
#include <math.h>
#include <cassert>

const Vec3f Vec3f::POS_Y( 0, 1, 0);
const Vec3f Vec3f::NEG_Y( 0, -1, 0);
const Vec3f Vec3f::POS_X( 1, 0, 0);
const Vec3f Vec3f::NEG_X( -1, 0, 0);
const Vec3f Vec3f::POS_Z( 0, 0, 1 );
const Vec3f Vec3f::NEG_Z( 0, 0, -1);

const Vec3f Vec3f::ZERO( 0.0f, 0.0f, 0.0f );

const Vec3f Vec3f::WHITE( 1.0f, 1.0f, 1.0f );
const Vec3f Vec3f::BLACK( 0.0f, 0.0f, 0.0f );
const Vec3f Vec3f::RED( 1.0f, 0.0f, 0.0f );
const Vec3f Vec3f::GREEN( 0.0f, 1.0f, 0.0f );
const Vec3f Vec3f::BLUE( 0.0f, 0.0f, 1.0f );
const Vec3f Vec3f::YELLOW( 1.0f, 1.0f, 0.0f );
const Vec3f Vec3f::CYAN( 0.0f, 1.0f, 1.0f );
const Vec3f Vec3f::VIOLET( 1.0f, 0.0f, 1.0f );
const Vec3f Vec3f::DARK_VIOLET( 0.58f, 0.0f, 0.827f );  // 0x9400D3

Vec3f::Vec3f()
{
	clear();
}

// -------------------------------------------------------------
Vec3f::Vec3f( const Vec3f& other )
{
	set( other );

}

// -------------------------------------------------------------
Vec3f::Vec3f( float x, float y, float z )
{ 
	set( x, y, z );
}

// -------------------------------------------------------------
void Vec3f::set( float x, float y, float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
}

// -------------------------------------------------------------
void Vec3f::set( const Vec3f & other )
{
	set( other.x, other.y, other.z );
}

// -------------------------------------------------------------
void Vec3f::setBytes( unsigned char r, unsigned char g, unsigned char b )
{ 
	set( (float) r / 255.0, (float) g / 255.0, (float) b / 255.0 );
}

// -------------------------------------------------------------
void Vec3f::setHex( int h ) 
{
	unsigned char r = ( h >> 16 ) & 0xFF;
	unsigned char g = ( h >> 8 ) & 0xFF;
	unsigned char b = h & 0xFF;

	setBytes( r, g, b );
}

// -------------------------------------------------------------
//http://stackoverflow.com/questions/30011741/3d-vector-defined-by-2-angles
void Vec3f::fromAngles( float yaw, float pitch )
{
 	float yawRad   = GameMath::degToRad( yaw );
 	float pitchRad = GameMath::degToRad( pitch  );

	this->x = cos( yawRad ) * cos( pitchRad );
	this->z = sin( yawRad ) * cos( pitchRad );
	this->y = sin( pitchRad );
}

// -------------------------------------------------------------
void Vec3f::add( float x, float y, float z )
{ 
	this->x += x;
	this->y += y;
	this->z += z;
}

// -------------------------------------------------------------
void Vec3f::sub( const Vec3f& other )
{ 
    this->x -= other.x;
    this->y -= other.y;
    this->z -= other.z;
}

// -------------------------------------------------------------
void Vec3f::add( const Vec3f & other )
{
	add( other.x, other.y, other.z );
}

// -------------------------------------------------------------
void Vec3f::clear()
{ 
	set( 0.0f, 0.0f, 0.0f );
}

// -------------------------------------------------------------
void Vec3f::scale( float s )
{
	this->x *= s;
	this->y *= s;
	this->z *= s;
}

// -------------------------------------------------------------
void Vec3f::scale( float x, float y, float z )
{
	this->x *= x;
	this->y *= y;
	this->z *= z;
}

// -------------------------------------------------------------
Vec3f Vec3f::modulate( const Vec3f& v ) const
{
    return( Vec3f( this->x * v.x, this->y * v.y, this->z * v.z ) );

}

// -------------------------------------------------------------
Vec3f Vec3f::times( float val ) const
{
	return Vec3f( x * val, y * val, z * val );
}

// -------------------------------------------------------------
float Vec3f::length()
{
	return sqrt( this->x * this->x + this->y * this->y + this->z * this->z );
}

// -------------------------------------------------------------
void Vec3f::normalize()
{
	double len = length();
	this->x /= len;
	this->y /= len;
	this->z /= len;
}

// -------------------------------------------------------------
Vec3f Vec3f::normalized() const
{
	Vec3f n( this->x, this->y, this->z );
	n.normalize();
	return n;
}

// -------------------------------------------------------------
void Vec3f::clampToMax( float a, float b, float c )
{
	this->x = std::min( this->x, a );
	this->y = std::min( this->y, b );
	this->z = std::min( this->z, c );
}

// -------------------------------------------------------------
float Vec3f::min() const
{
	return GameMath::minf( GameMath::minf( x, y ), z );
}

// -------------------------------------------------------------
float Vec3f::max() const
{
	return GameMath::maxf( GameMath::maxf( x, y ), z );	
}

// -------------------------------------------------------------
Vec3f Vec3f::plus( const Vec3f& v ) const
{ 
	return Vec3f( x + v.x, y + v.y, z + v.z );
}

// -------------------------------------------------------------
Vec3f Vec3f::plus( float x, float y, float z) const
{
    return Vec3f( this->x + x, this->y + y, this->z + z );
}

// -------------------------------------------------------------
Vec3f Vec3f::minus( const Vec3f& v ) const
{ 
	return Vec3f( x - v.x, y - v.y, z - v.z );
}

// -------------------------------------------------------------
Vec3f Vec3f::inverse() const
{
    return Vec3f( -x, -y, -z );
}

// -------------------------------------------------------------
int Vec3f::classify( const Vec3f& v )
{
    Vec3f vn( v.normalized() );

    if( Vec3f::dot( vn, Vec3f::POS_Y ) >= DIRECTION_THRESHOLD ) return POS_Y_DIR;
    if( Vec3f::dot( vn, Vec3f::NEG_Y ) >= DIRECTION_THRESHOLD ) return NEG_Y_DIR;
    if( Vec3f::dot( vn, Vec3f::POS_X ) >= DIRECTION_THRESHOLD ) return POS_X_DIR;
    if( Vec3f::dot( vn, Vec3f::NEG_X ) >= DIRECTION_THRESHOLD ) return NEG_X_DIR;
    if( Vec3f::dot( vn, Vec3f::POS_Z ) >= DIRECTION_THRESHOLD ) return POS_Z_DIR;
    if( Vec3f::dot( vn, Vec3f::NEG_Z ) >= DIRECTION_THRESHOLD ) return NEG_Z_DIR;

    printf("WARNING: cannot classify %f %f %f\n", v.x, v.y, v.z );
    return UNKNOWN_DIR;
}

// -------------------------------------------------------------
Vec3f Vec3f::center2( const Vec3f& a, const Vec3f& b )
{
   return Vec3f( (a.x + b.x) / 2.0f, 
                 (a.y + b.y) / 2.0f,
                 (a.z + b.z) / 2.0f ); 
}

// -------------------------------------------------------------
Vec3f Vec3f::center3( const Vec3f& a, const Vec3f& b, const Vec3f& c )
{
    return Vec3f( (a.x + b.x + c.x) / 3.0f, 
                  (a.y + b.y + c.y) / 3.0f,
                  (a.z + b.z + c.z) / 3.0f );

}

// -------------------------------------------------------------
float Vec3f::dot( const Vec3f& a, const Vec3f& b )
{
	return( a.x * b.x + a.y * b.y + a.z * b.z );
}

// -------------------------------------------------------------
Vec3f Vec3f::cross( const Vec3f& a, const Vec3f& b, const Vec3f& c )
{
    Vec3f ab( b.minus( a ) );
    ab.normalize();

    Vec3f ac( c.minus( a ) );
    ac.normalize();

    return Vec3f::cross( ac, ab );
}

// -------------------------------------------------------------
Vec3f Vec3f::cross( const Vec3f& a, const Vec3f& b ) 
{
    return Vec3f( a.y * b.z - a.z * b.y,
                  a.z * b.x - a.x * b.z,
                  a.x * b.y - a.y * b.x );
}

// -------------------------------------------------------------
float Vec3f::distance( const Vec3f& a, const Vec3f& b)
{
	Vec3f vToLight( b.x - a.x, b.y - a.y, b.z - a.z );
	return vToLight.length();
}

//  ------------------------------------------------------------
float Vec3f::distAtten( const Vec3f& a, const Vec3f &b )
{
	float dist = Vec3f::distance( a, b );
	return( 1.0f / (dist * dist ) );
}

//  ------------------------------------------------------------
Vec3f Vec3f::lerp( const Vec3f& a, const Vec3f& b, float t )
{
	return Vec3f( GameMath::lerp( a.x, b.x, t ),
				  GameMath::lerp( a.y, b.y, t ),
			      GameMath::lerp( a.z, b.z, t ) );
}

///  ------------------------------------------------------------
// https://keithmaggio.wordpress.com/2011/02/15/math-magician-lerp-slerp-and-nlerp/
Vec3f Vec3f::slerp( const Vec3f& a, const Vec3f& b, float t )
{
	 if( a.x == b.x && a.y == b.y && a.z == b.z ) return b;

     float dot = Vec3f::dot( a, b );     
     dot = GameMath::clampf( dot, -1.0f, 1.0f );

     // Acos(dot) returns the angle between start and end,
     // And multiplying that by percent returns the angle between
     // start and the final result.
     float theta = acos(dot)*t;

     Vec3f rel( b.x - a.x * dot, b.y - a.y * dot, b.z - a.z * dot );
     rel.normalize();     // Orthonormal basis

     rel.scale( sin( theta ) );
     // The final result.

     Vec3f result( a.x, a.y, a.z );
     result.scale( cos( theta ) );
     result.add( rel.x, rel.y, rel.z );

     return result;
}

// -------------------------------------------------------------
Vec3f Vec3f::cosWeightedHemisphereWithin( const Vec3f& n, float dot )
{
    // don't bother if need an exact value!
    if( dot == 1.0f ) return n;


    Vec3f ret = Vec3f::cosWeightedHemisphere( n );
    while( Vec3f::dot( ret, n ) < dot )
    {
        ret = Vec3f::cosWeightedHemisphere( n );
    }

    return ret;
}


// -------------------------------------------------------------
// https://pathtracing.wordpress.com/2011/03/03/cosine-weighted-hemisphere/
Vec3f Vec3f::cosWeightedHemisphere( const Vec3f& n )
{
    float Xi1 = (float)rand() / (float)RAND_MAX;
    float Xi2 = (float)rand() / (float)RAND_MAX;

    float  theta = acos( sqrt(1.0 - Xi1 ) );
    float  phi = 2.0 * PI * Xi2;

    float xs = sinf(theta) * cosf(phi);
    float ys = cosf(theta);
    float zs = sinf(theta) * sinf(phi);

    Vec3f y( n.x, n.y, n.z );
    Vec3f h( n.x, n.y, n.z );

    if (fabs(h.x)<=fabs(h.y) && fabs(h.x)<=fabs(h.z))
        h.x= 1.0;
    else if (fabs(h.y)<=fabs(h.x) && fabs(h.y)<=fabs(h.z))
        h.y= 1.0;
    else
        h.z= 1.0;

    Vec3f x = Vec3f::cross( h, y ).normalized();
    Vec3f z = Vec3f::cross( x, y ).normalized();

    Vec3f result;
    result.add( x.x * xs, x.y * xs, x.z * xs );
    result.add( y.x * ys, y.y * ys, y.z * ys );
    result.add( z.x * zs, z.y * zs, z.z * zs );

    return result.normalized();
}

// -------------------------------------------------------------
hsv Vec3f::rgbToHSV( const Vec3f& in)
{
    hsv         out;

    double min = in.min();
    double max = in.max();

    out.v = max;                                // v
    double delta = max - min;
    if (delta < 0.00001f)
    {
        out.s = 0.0f;
        out.h = 0.0f; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0f ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
        // s = 0, v is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if( in.r() >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g() - in.b() ) / delta;        // between yellow & magenta
    else
    if( in.g() >= max )
        out.h = 2.0 + ( in.b() - in.r() ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r() - in.g() ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}

// -------------------------------------------------------------
Vec3f Vec3f::hsvToRGB( const hsv& in )
{
    double      hh, p, q, t, ff;
    long        i;
    Vec3f       out;

    if(in.s <= 0.0f)     // < is bogus, just shuts up warnings
    {       
    	out.set( in.v, in.v, in.v );
        return out;
    }

    hh = in.h;
    if(hh >= 360.0f) hh = 0.0f;
    hh /= 60.0f;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch( i )
    {
	    case 0:
	    	out.set( in.v, t, p );
	        break;
	    case 1:
	    	out.set( q, in.v, p );
	        break;
	    case 2:
	    	out.set( p, in.v, t );
	        break;
	    case 3:
	    	out.set( p, q, in.v );
	        break;
	    case 4:
	    	out.set( t, p, in.v );
	        break;
	    case 5:
	    default:
	    	out.set( in.v, p, q );
	        break;
    }
    return out;     
}