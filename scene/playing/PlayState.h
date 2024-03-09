#pragma once

#include <vector>
#include "../../world/Patch.h"
#include "../../util/Vec3i.h"

class Game;

class Move
{
public:
	Vec3i loc;
	int dir;
	int materialId;

	Move( const Vec3i& loc, int dir, int materialId );
};


class PlayState
{
private:
	Game *            mGame;
	Patch *           mEtherStart;
	std::vector<Move> mMoves;
	std::vector<Move> mFutureMoves;

	int mEtherAvailable;
	int mEtherUsed;

	void clear();
	int topMoveCount( int materialId );
	void generateStartMoves();
	void generateFutureMoves();
	bool alreadyMovedAt( const Vec3i& loc, int dir );
	bool hasFutureMove( const Vec3i& loc, int dir );
	Patch * findNearbyEther( Patch *patch );
	int countEtherPatches( int groupId );

public:
	PlayState();

	void init( Game* game );
	void render();
	void interact( Patch *p );
};