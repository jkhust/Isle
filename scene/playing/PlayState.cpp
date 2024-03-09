#include "PlayState.h"
#include "../../game/Game.h"
#include "../../render/RenderManager.h"
#include "../../sound/SoundManager.h"
#include "../../world/World.h"
#include "../../util/Vec3f.h"


Move::Move( const Vec3i& loc, int dir, int materialId )
{
	this->loc.set( loc );
	this->dir = dir;
	this->materialId = materialId;
}

// --------------------------------------------------------------------------
PlayState::PlayState()
{
	
}

// --------------------------------------------------------------------------
void PlayState::init( Game* game )
{
	mGame = game;
	clear();
}

// --------------------------------------------------------------------------
void PlayState::clear()
{
	mEtherStart = NULL;
	mMoves.clear();
	mFutureMoves.clear();
	generateStartMoves();
}

// --------------------------------------------------------------------------
void PlayState::render()
{
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE);    

	// render moves as quads of light.
	for( int i=0; i < mMoves.size(); ++i )
	{
		Vec3f pos = mGame->world()->locToCenterPos( mMoves[i].loc );
		pos.y += World::HALF_BLOCK_SIZE + 0.001;
		mGame->renderManager()->renderSolidQuad( Vec3f( pos.x - World::HALF_BLOCK_SIZE, pos.y, pos.z - World::HALF_BLOCK_SIZE ),
												 Vec3f( pos.x + World::HALF_BLOCK_SIZE, pos.y, pos.z - World::HALF_BLOCK_SIZE ),
										  	     Vec3f( pos.x + World::HALF_BLOCK_SIZE, pos.y, pos.z + World::HALF_BLOCK_SIZE ),
										  	     Vec3f( pos.x - World::HALF_BLOCK_SIZE, pos.y, pos.z + World::HALF_BLOCK_SIZE ),
										  	     Vec3f( 1.0f, 1.0f, 1.0f), 0.5f
											   );
	}

	// render future moves as outlines.
	for( int i=0; i < mFutureMoves.size(); ++i )
	{
		Vec3f pos = mGame->world()->locToCenterPos( mFutureMoves[i].loc );
		pos.y += World::HALF_BLOCK_SIZE + 0.001;
		mGame->renderManager()->renderWireQuad( Vec3f( pos.x - World::HALF_BLOCK_SIZE, pos.y, pos.z - World::HALF_BLOCK_SIZE ),
									  	        Vec3f( pos.x + World::HALF_BLOCK_SIZE, pos.y, pos.z - World::HALF_BLOCK_SIZE ),
										  	    Vec3f( pos.x + World::HALF_BLOCK_SIZE, pos.y, pos.z + World::HALF_BLOCK_SIZE ),
										  	    Vec3f( pos.x - World::HALF_BLOCK_SIZE, pos.y, pos.z + World::HALF_BLOCK_SIZE ),
										  	    Vec3f( 1.0f, 1.0f, 1.0f), 0.5f, 2.0f
											  );
	}
}

// --------------------------------------------------------------------------
/*
	Possibilities:
		1) Traverse Ether - At any time, you can click an ether block to traverse.
		2) Traverse Void  - If void is in your future moves, you are allowed to traverse there.

		3) Start          - If you have no moves, you can click next to ether to begin moving.
		4) Backtrack      - If you click somewhere you've already moved, then you backtrack to there.
		5) Move           - click a block in future moves to generate OTHER future moves.
		6) Cancel         - Have moves, and click an empty space to cancel.

		SFX:
			moveAccepted  - The Witness "clink" sound.
			moveRejected  - The Witness "bad" sound.
			moveCancel    - MP Morph Ball SFX.

			etherAccepted - "Fwoosh to white" from BotW.
			etherRejected - Star Trek force field?

			voidAccepted  - DSouls "Traverse the White Light"
			voidRejected  - Dunno.
*/
void PlayState::interact( Patch *p )
{
	Vec3i loc = mGame->world()->patchLoc( p );

	// 1) Traverse ETHER
	if( p->materialId == Material::ETHER )
	{
		mGame->fader().fade( Vec3f::WHITE, 0.5f, 0.0f, 0.50f );
		printf("SOUND: etherRejected\n");

		// Script *s = smMap->getScript( p->scriptId );
		// if( s )
		// {
		// 	mGame->resourceManager()->playSound("etherAccepted");
		// 	mGame->runCommand( s->data );		
		// }	
		// else
		// {
		// 	mGame->resourceManager()->playSound("etherRejected");
		// }	
	}
	// 2) Traverse VOID
	else if( p->materialId == Material::VOID )
	{
		mGame->fader().fade( Vec3f::DARK_VIOLET, 0.5f, 0.0f, 0.50f );  // dark violet
		mGame->soundManager()->playSound("void_success");

		// if( hasFutureMove( p )  )
		// {
		// 	Script *s = mMap->getScript( p->scriptId );
		// 	if( s != null )
		// 	{
		// 		mGame->resourceManager()->playSound("voidAccepted");
		// 		mGame->runCommand( s->data );		
		// 	}
		// 	else
		// 	{
		// 		mGame->resourceManager()->playSound("voidRejected");
		// 	}
		// }
		// else
		// {
		// 	mGame->resourceManager()->playSound("voidRejected");
		// }
	}
	// 3) *** backtrack ***
	else if( alreadyMovedAt( loc, p->dir ) )
	{
		// backtrack to the marker clicked on
		int i = mMoves.size() - 1;
		while( i >= 0 && !(mMoves[i].loc.equals(loc) && mMoves[i].dir == p->dir ) )
		{
			mEtherUsed -= 2;
			mMoves.erase( mMoves.begin() + i );
			--i;
		}
		generateFutureMoves();
		//mGame->resourceManager()->playSound("moveAccepted");
		mGame->soundManager()->playSound("move_rejected");

	}
	// 4) move
	else if( hasFutureMove( loc, p->dir ) )
	{
		// count how many moves you have.
		if( mMoves.size() == 0 )
		{
			Patch * etherPatch = findNearbyEther( p );
			assert( etherPatch ); // we should have this because that's how we generated the move.			
			mEtherAvailable = countEtherPatches( 1 ); /* etherPatch->groupId */
			mEtherUsed = 0;			
		}

		mEtherUsed += 2;
		mMoves.push_back( Move( loc, p->dir, p->materialId ) );
		generateFutureMoves();
		mGame->soundManager()->playSound("move_accepted");
	}
	// not a future move, so cancel.
	else
	{
		if( mMoves.size() > 0 )
		{
			clear();
		}
		mGame->soundManager()->playSound("moves_cleared");
	}
}

// -------------------------------------------------------------------------
int PlayState::countEtherPatches( int groupId )
{
	std::vector<Patch *>& patches = mGame->world()->patches();
	int count = 0;

	for( int i=0; i < patches.size(); ++i )
	{
		Patch *patch = patches[i];
		if( patch->materialId == Material::ETHER )
		{
			++count;
		}
	}

	return count;
}

// -------------------------------------------------------------------------
Patch * PlayState::findNearbyEther( Patch *patch )
{
	Vec3i loc = mGame->world()->patchLoc( patch );

	Patch *etherPatch = mGame->world()->getPatchWithinLoc( loc.minus(1, 0, 1), loc.plus( 1, 0, 1 ), Material::ETHER, Vec3f::POS_Y_DIR );
	if( etherPatch )
		return etherPatch;

	etherPatch = mGame->world()->getPatchWithinLoc( loc.plus( 0, 1, 0), loc.plus( 0, 1, 0), Material::ETHER, Vec3f::POS_Z_DIR | Vec3f::POS_X_DIR );
	if( etherPatch )
		return etherPatch;

	return NULL;
}

// -------------------------------------------------------------------------
void PlayState::generateStartMoves()
{
	std::vector<Patch *>& patches = mGame->world()->patches();

	// for each patch in world, if non-ether and non-void, see if any ether next to it.
	for( int i=0; i < patches.size(); ++i )
	{
		Patch *patch = patches[i];
		if( patch->materialId != Material::ETHER && patch->materialId != Material::VOID )
		{
			if( findNearbyEther( patch ) ) 
			{
				Vec3i loc = mGame->world()->patchLoc( patch );
				mFutureMoves.push_back( Move( loc, patch->dir, patch->materialId ) );
			}
		}
	}
}

// -------------------------------------------------------------------------
void PlayState::generateFutureMoves()
{	
	if( mMoves.size() == 0 )
	{
		printf("WARNING: gen future moves failed due to no moves.\n");
		return;
	}

	mFutureMoves.clear();
	// out of ether, can't move again.
	if( mEtherUsed >= mEtherAvailable ) return;

 	// *** do some pre-analysis ***
	int oceanCount = topMoveCount( Material::OCEAN );
	int oceanMoveDist = (oceanCount % 3 == 0) ? 1 : 2;

	// OCEAN step/dist:
	// 0       3        6
	// 1 2, 2, 1, 2, 2, 1, 2, 2, 1

	Move& topMove = mMoves[ mMoves.size() - 1 ];
	Vec3f topMoveSightPos = mGame->world()->locToCenterPos( topMove.loc.plus( 0, 1, 0 ) );

	std::vector<Patch *>& patches = mGame->world()->patches();
	for( int i=0; i < patches.size(); ++i )
	{
		Patch *p = patches[i];
		Vec3i loc = mGame->world()->patchLoc( p );
		Vec3f sightPos = mGame->world()->locToCenterPos( loc.plus( 0, 1, 0 ) );

		// only count non-ether moves we haven't already moved at.
		if( p->materialId != Material::ETHER &&
			!alreadyMovedAt( loc, p->dir ) &&
			!hasFutureMove( loc, p->dir ) )		
		{
			Vec3i dists( abs( topMove.loc.i - loc.i ), abs( topMove.loc.j - loc.j ), abs( topMove.loc.k - loc.k ) );

			// *** check for ocean ***
			if( p->dir == Vec3f::POS_Y_DIR && 
				dists.j == 0 && ( ( dists.i == oceanMoveDist && dists.k == 0 ) || ( dists.k == oceanMoveDist && dists.i == 0 ) ) 
				&& mGame->world()->lineOfSight( topMoveSightPos, sightPos )
				)
			{
				mFutureMoves.push_back( Move( loc, p->dir, p->materialId ) );
			}
		}
	}
}

// --------------------------------------------------------------------------
int PlayState::topMoveCount( int materialId )
{
	int count = 0;

	int i = mMoves.size() - 1;
	while( i >= 0 && mMoves[i].materialId == materialId )
	{
		++count;
		--i;
	}
	return count;
}

// --------------------------------------------------------------------------
bool PlayState::alreadyMovedAt( const Vec3i& loc, int dir )
{
	for( int i=0; i < mMoves.size(); ++i )
	{
		if( mMoves[i].loc.equals( loc ) && mMoves[i].dir == dir )
		{
			return true;
		}
	}

	return false;
}

// --------------------------------------------------------------------------
bool PlayState::hasFutureMove( const Vec3i& loc, int dir )
{
	for( int i=0; i < mFutureMoves.size(); ++i )
	{
		if( mFutureMoves[i].loc.equals( loc ) && mFutureMoves[i].dir == dir )
		{
			return true;
		}
	}

	return false;
}