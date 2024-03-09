#include "Ray.h"

Ray::Ray( const Vec3f& start, const Vec3f& end )
{
	this->start.set( start );
	this->end.set( end );

	dir.set( end.minus( start ).normalized() );
	dir_inv.set( dir.inverse() );

	// mask out any directions we can't possibly collide with
	hitFlags = Vec3f::ALL_DIRS;
	hitFlags &= ~(dir.x >= 0 ? Vec3f::POS_X_DIR : Vec3f::NEG_X_DIR );
	hitFlags &= ~(dir.y >= 0 ? Vec3f::POS_Y_DIR : Vec3f::NEG_Y_DIR );
	hitFlags &= ~(dir.z >= 0 ? Vec3f::POS_Z_DIR : Vec3f::NEG_Z_DIR );
}