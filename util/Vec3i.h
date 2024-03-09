#pragma once

class Vec3i
{
public:
	int i, j, k;

	Vec3i();
	Vec3i( int i, int j, int k );

	void set( int i, int j, int k );
	void set( const Vec3i& v );
	void clear();
	Vec3i plus( const Vec3i& v ) const;
	Vec3i minus( const Vec3i& v ) const;
	Vec3i minus( int i, int j, int k ) const;
	Vec3i plus( int i, int j, int k ) const;
	Vec3i plus( float x, float y, float z ) const;
	bool equals( const Vec3i& v ) const;
};
