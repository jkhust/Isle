#include "Vec3i.h"
	
#include <math.h>

Vec3i::Vec3i()
{
	clear();
}

// -------------------------------------------------------------
Vec3i::Vec3i( int i, int j, int k)
{ 
	set( i, j, k );
}

// -------------------------------------------------------------
void Vec3i::set( int i, int j, int k )
{
	this->i = i; this->j = j; this->k = k;
}

// -------------------------------------------------------------
void Vec3i::set( const Vec3i& v )
{
	set( v.i, v.j, v.k );
}

// -------------------------------------------------------------
void Vec3i::clear()
{
	set( 0, 0, 0 );
}

// -------------------------------------------------------------
Vec3i Vec3i::plus( const Vec3i& v ) const
{
	return Vec3i( i + v.i, j + v.j, k + v.k);
}

// -------------------------------------------------------------
Vec3i Vec3i::minus( const Vec3i& v ) const
{
	return Vec3i( i - v.i, j - v.j, k - v.k);
}

// -------------------------------------------------------------
Vec3i Vec3i:: minus( int i, int j, int k ) const
{
	return( Vec3i( this->i - i, this->j - j, this->k - k ) );

}

// -------------------------------------------------------------
Vec3i Vec3i::plus( int i, int j, int k ) const
{
	return( Vec3i( this->i + i, this->j + j, this->k + k ) );
}

// -------------------------------------------------------------
Vec3i Vec3i::plus( float x, float y, float z ) const
{
	return Vec3i( i + floor(x), j + floor(y), k + floor(z) );
}

// -------------------------------------------------------------
bool Vec3i::equals( const Vec3i& v ) const
{
	return ( this->i == v.i && this->j == v.j && this->k == v.k );
}

