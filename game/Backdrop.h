#pragma once

#include "../util/Vec3f.h"
#include <vector>

class Game;

class BackdropStop
{
public:
	float percent;
	Vec3f color;

	BackdropStop( float percent, const Vec3f& color )
	{
		this->percent = percent;
		this->color.set( color );
	}
};

class Backdrop
{
private:
	Game*  mGame;
	bool   mVisible;
	std::vector<BackdropStop> mStops;

public:
	bool visible() 				 	{ return mVisible; }
	void setVisible( bool value ) 	{ mVisible = value; }
	// for interactive editing.
	bool hasIndex( int index ) 		{ return index >= 0 && index < mStops.size(); }
	BackdropStop& stop( int index ) { return mStops[index]; }
	int  stopCount() 				{ return mStops.size(); }

	Backdrop();
	void init( Game *game );
	void clear();
	void add( float percent, const Vec3f& color );
	void render();
};