#pragma once

class hsv
{
public:
	float h;
	float s;
	float v;
};

class Vec3f
{
public:
	constexpr static const float PI = 3.1415926535897932384626433832795f;
	
	constexpr static const float DIRECTION_THRESHOLD = 0.98;
	static const int   UNKNOWN_DIR = 0;
	static const int   POS_Y_DIR = ( 1 << 0 );
	static const int   NEG_Y_DIR = ( 1 << 1 );
	static const int   POS_X_DIR = ( 1 << 2 );
	static const int   NEG_X_DIR = ( 1 << 3 );
	static const int   POS_Z_DIR = ( 1 << 4 );
	static const int   NEG_Z_DIR = ( 1 << 5 );
	static const int   ALL_DIRS  = 0x3f;

	static const Vec3f POS_Y;
	static const Vec3f NEG_Y;
	static const Vec3f POS_X;
	static const Vec3f NEG_X;
	static const Vec3f POS_Z;
	static const Vec3f NEG_Z;

	static const Vec3f ZERO;
	static const Vec3f WHITE;
	static const Vec3f BLACK;
	static const Vec3f RED;
	static const Vec3f GREEN;
	static const Vec3f BLUE;
	static const Vec3f YELLOW;
	static const Vec3f CYAN;
	static const Vec3f VIOLET;
	static const Vec3f DARK_VIOLET;

	float x, y, z;

	float r() const { return x; }
	float g() const { return y; }
	float b() const { return z; }

	Vec3f();
	Vec3f( const Vec3f& other );
	Vec3f( float x, float y, float z );

 	void set( float x, float y, float z );
 	void set( const Vec3f & other );
 	void fromAngles( float yaw, float pitch );

	void setBytes( unsigned char r, unsigned char g, unsigned char b );
	void setHex( int h );

	void add( float x, float y, float z );
	void add( const Vec3f & other );
	void sub( const Vec3f & other );

	void clear();
	void scale( float s );
	void scale( float x, float y, float z );
	Vec3f modulate( const Vec3f& v ) const;
	Vec3f times( float val ) const;
	Vec3f plus( const Vec3f& v ) const;
	Vec3f plus( float x, float y, float z) const;
	Vec3f minus( const Vec3f& v ) const;
	Vec3f inverse() const;
	float length();
	void normalize();
	Vec3f normalized() const;
	void clampToMax( float a, float b, float c );
	float min() const;
	float max() const;

	static int classify( const Vec3f& v );
	static Vec3f center2( const Vec3f& a, const Vec3f& b );
	static Vec3f center3( const Vec3f& a, const Vec3f& b, const Vec3f& c );
	static float dot( const Vec3f& a, const Vec3f& b );
	static Vec3f cross( const Vec3f& a, const Vec3f& b, const Vec3f& c );
	static Vec3f cross( const Vec3f& a, const Vec3f& b );
	static float distance( const Vec3f& a, const Vec3f& b );
	static float distAtten( const Vec3f& a, const Vec3f& b );
	static Vec3f cosWeightedHemisphereWithin( const Vec3f &n, float dot );
	static Vec3f cosWeightedHemisphere( const Vec3f& n );
	static Vec3f lerp( const Vec3f& a, const Vec3f& b, float t );
	static Vec3f slerp( const Vec3f& a, const Vec3f& b, float t );

	static hsv rgbToHSV( const Vec3f& in );
	static Vec3f hsvToRGB( const hsv& in );

};