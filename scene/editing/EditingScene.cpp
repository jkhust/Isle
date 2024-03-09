#include "EditingScene.h"

#include <string>
#include <SDL.h>
#include <sstream>
#include <vector>

#include "../../game/Game.h"
#include "../../game/Console.h"
#include "../../input/InputManager.h"
#include "../../render/RenderManager.h"
#include "../../map/MaterialManager.h"
#include "../../util/GameMath.h"
#include "../../util/StringUtil.h"
#include "../../util/Ray.h"
#include "../../world/World.h"
#include "../../world/Light.h"

EditingScene::EditingScene( Game * game ) : Scene( game )
{
	mCamera.init( this, Camera::PERSPECTIVE );

	mFSM.init( this );
	mFSM.addState( EDIT_MODE, &EditingScene::whileEditing );
	mFSM.addState( CONSOLE_MODE, &EditingScene::whileInConsole );
	mFSM.addState( BACKDROP_MODE, &EditingScene::whileEditingBackdrop );

	mFSM.addTransition( EDIT_MODE, CONSOLE_MODE );
	mFSM.addTransition( CONSOLE_MODE, EDIT_MODE );
	mFSM.addTransition( CONSOLE_MODE, BACKDROP_MODE );
	mFSM.addTransition( BACKDROP_MODE, EDIT_MODE );

	mConsole = new Console( game );
	mCurrMaterial = 0;
	mCurrMaterial = Material::OCEAN;
}

// ----------------------------------------------------------------------------
void EditingScene::activate()
{
	Scene::activate();
	mGame->renderManager()->setClearColor( 0.2f, 0.2f, 0.2f );
	mCamera.apply();
}

// ----------------------------------------------------------------------------
void EditingScene::whileEditing( double dt )
{
	allowEnterConsole();

	allowPause();
	allowCycleCurrentMaterial();
	allowCycleCurrentLight();
	allowAdjustCamera( dt );

	allowAddBlocks();
	allowAddRemovePatches();
	allowUpdateMaterial();
	allowEditLight();
}

// ----------------------------------------------------------------------------
void EditingScene::allowCycleCurrentLight() 
{
	InputEvent e;
	if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_LEFTBRACKET ) )
	{
		--mCurrLight;
	}
	else if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_RIGHTBRACKET ) )
	{
		++mCurrLight;
	}
	mCurrLight = GameMath::clampi( mCurrLight, 0, MAX_LIGHTS - 1 );
}

// ----------------------------------------------------------------------------
void EditingScene::allowEditLight() 
{
	bool show = false;

	Light *l = mGame->world()->getLight( mCurrLight );
	Vec3f scenePos = mGame->inputManager()->scenePos();
	Patch *p = mGame->world()->getPatchAt( scenePos );
	bool mouseMoved = mGame->inputManager()->mouseMoved();

	// *** intensity ***
	if( mGame->inputManager()->keyHeld( SDLK_i ) )
	{
		if( mouseMoved )
		{
			l->intensity = GameMath::maxf( l->intensity + mGame->inputManager()->mouseDX() * 10.0f, 0.0f );
		}
		show = true;
	}

	// *** position ***
	if( mGame->inputManager()->keyHeld( SDLK_p ) )
	{
		if( p != NULL && mouseMoved )
		{
			l->pos.x = scenePos.x;
			l->pos.z = scenePos.z;
		}
		show = true;
	}
	// *** height ***
	else if( mGame->inputManager()->keyHeld( SDLK_y ) )
	{
		if( mouseMoved )
		{
			l->pos.y += mGame->inputManager()->mouseDY() * -2.0f;
		}
		show = true;
	}
	// *** move ***
	else if( mGame->inputManager()->keyHeld( SDLK_m ) )
	{
		if( mouseMoved )
		{
			l->pos.z += mGame->inputManager()->mouseDX() * 2.0f;
			l->pos.x += mGame->inputManager()->mouseDY() * 2.0f;
		}
		show = true;
	}
	// *** HS color ***
	else if( mGame->inputManager()->keyHeld( SDLK_h ) )
	{
		if( mouseMoved )
		{
			// convert to hsv, adjust h and s, then convert back to RGB
			hsv h = Vec3f::rgbToHSV( l->color );

			float hAdjust = mGame->inputManager()->mouseDX();
			float sAdjust = mGame->inputManager()->mouseDY() * -0.001;

			h.h = GameMath::clampf( h.h + hAdjust, 0.0f, 359.0f );
			h.s = GameMath::clampf( h.s + sAdjust, 0.0f, 1.0f );

			Vec3f newColor( Vec3f::hsvToRGB( h ) );
			l->color.set( newColor );		
		}
		show = true;
	}
	// *** V color ***
	else if( mGame->inputManager()->keyHeld( SDLK_v ) )
	{
		if( mouseMoved )
		{
			// convert to hsv, adjust v, then convert back to RGB
			hsv h = Vec3f::rgbToHSV( l->color );

			float vAdjust = mGame->inputManager()->mouseDY() * -0.001;

			h.v = GameMath::clampf( h.v + vAdjust, 0.0f, 1.0f );

			Vec3f newColor( Vec3f::hsvToRGB( h ) );
			l->color.set( newColor );			
		}
		show = true;
	}
	else if( mGame->inputManager()->keyHeld( SDLK_g ) )
	{
		if( p != NULL && mouseMoved )
		{
			l->end.x = scenePos.x;
			l->end.y = scenePos.y;
			l->end.z = scenePos.z;
		}
		show = true;
	}
	else if( mGame->inputManager()->keyHeld( SDLK_t ) )
	{
		if( mouseMoved )
		{
			l->end.y += mGame->inputManager()->mouseDY() * -2.0f;
		}
		show = true;
	}

	InputEvent e;
	if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_o ) )
	{
		l->enabled = !l->enabled;
		show = true;
	}

	if( show )
	{
		// popsicle stick for light.
		mGame->renderManager()->renderWireSphere( l->pos, l->color, 8 );
		Vec3f end( l->pos.x, 0.0f, l->pos.z );
		mGame->renderManager()->renderLine( l->pos, Vec3f::WHITE, end, Vec3f::WHITE );

		// and additional direction if dir ligt.
		if( l->type == Light::DIRECTIONAL )
		{
			mGame->renderManager()->renderLine( l->pos, Vec3f::YELLOW, l->end, Vec3f::YELLOW );
		}
	}
}

// ----------------------------------------------------------------------------
void EditingScene::whileInConsole( double dt )
{
	if( mFSM.timeInState() == 0.0f )
	{
		mConsole->enter();
	}

	InputEvent e;
	if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_RETURN ) )
	{		
		mFSM.transitionTo( mFSM.lastState() );
		// the command may change the state we transition to, so run this second.
		mConsole->submitCommand();
	}
	else if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_ESCAPE ) )
	{		
		mFSM.transitionTo( mFSM.lastState() );
	}
	else
	{
		mConsole->tick( dt );
		std::string commandToRender = "> " + mConsole->command();
		mGame->renderManager()->renderText( commandToRender.c_str(), 0, 0 );
	}
}

// ----------------------------------------------------------------------------
void EditingScene::beginEditingBackdrop( int index )
{
	if( mGame->backdrop().hasIndex( index ) )
	{
		// store mins and maxs for edit.
		mStopIndex = index;
		Backdrop& backdrop = mGame->backdrop();
		BackdropStop & editStop = backdrop.stop( mStopIndex );

		mStopMinPercent = ( mStopIndex > 0 ) ? backdrop.stop( mStopIndex - 1 ).percent + 0.01 : editStop.percent;
		mStopMaxPercent = ( mStopIndex < backdrop.stopCount() - 1 ) ? backdrop.stop( mStopIndex + 1 ).percent - 0.01 : editStop.percent;

		mFSM.transitionTo( BACKDROP_MODE );		
	}
	else
	{
		printf("Cannot edit: no backdrop index %d.\n", index );
	}
}

// ----------------------------------------------------------------------------
void EditingScene::whileEditingBackdrop( double dt )
{
	// H and V keys, or hold Y to position up/down.
	InputEvent e;
	if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_RETURN ) )
	{		
		mFSM.transitionTo( EDIT_MODE );
	}
	else if( mGame->inputManager()->keyHeld( SDLK_y ) && mGame->inputManager()->mouseMoved() )
	{
		BackdropStop& stop = mGame->backdrop().stop( mStopIndex );
		stop.percent += mGame->inputManager()->mouseDY() * 0.005f;

		stop.percent = GameMath::clampf( stop.percent, mStopMinPercent, mStopMaxPercent );
	}
	// *** HS color ***
	else if( mGame->inputManager()->keyHeld( SDLK_h ) && mGame->inputManager()->mouseMoved() )
	{
		BackdropStop& stop = mGame->backdrop().stop( mStopIndex );

		// convert to hsv, adjust h and s, then convert back to RGB
		hsv h = Vec3f::rgbToHSV( stop.color );

		float hAdjust = mGame->inputManager()->mouseDX();
		float sAdjust = mGame->inputManager()->mouseDY() * -0.001;

		h.h = GameMath::clampf( h.h + hAdjust, 0.0f, 359.0f );
		h.s = GameMath::clampf( h.s + sAdjust, 0.0f, 1.0f );
		stop.color = Vec3f::hsvToRGB( h );		
	}
	// *** V color ***
	else if( mGame->inputManager()->keyHeld( SDLK_v ) && mGame->inputManager()->mouseMoved() )
	{
		BackdropStop& stop = mGame->backdrop().stop( mStopIndex );

		// convert to hsv, adjust v, then convert back to RGB
		hsv h = Vec3f::rgbToHSV( stop.color );

		float vAdjust = mGame->inputManager()->mouseDY() * -0.001;

		h.v = GameMath::clampf( h.v + vAdjust, 0.0f, 1.0f );
		stop.color = Vec3f::hsvToRGB( h );
	}
}

// ----------------------------------------------------------------------------
void EditingScene::allowPause()
{
	InputEvent e;
	if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_SPACE ) )
	{
		mGame->world()->setPaused( !mGame->world()->paused() );
	}
}

// ----------------------------------------------------------------------------
void EditingScene::allowAddBlocks()
{
	InputEvent e;
	// *** add blocks ***
	if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_b ) )
	{		
		if( mGame->world()->patches().size() == 0 )
		{
			mGame->world()->addBlock( Vec3i(8, 0, 8 ), mCurrMaterial );
		}
		else
		{
			Patch *p = mGame->world()->getPatchAt( e.scenePos );
			if( p != NULL )
			{
				Vec3i loc = mGame->world()->locFromPos( p->center.plus( p->baseNormal ) );
				mGame->world()->addBlock( loc, mCurrMaterial );
			}			
		}
	}
}

// ----------------------------------------------------------------------------
void EditingScene::allowAddRemovePatches()
{	
	InputEvent e;
	// press c to record the plane patch to draw along.
	if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_c ) )
	{
		mPlanePatch = mGame->world()->getPatchAt( e.scenePos );
		if( mPlanePatch )
		{
			mCurrMaterial = mPlanePatch->materialId;
		}
	}
	// drag while holding c to create additional patches along the planepatch
	else if( mGame->inputManager()->keyHeld( SDLK_c ) && mGame->inputManager()->mouseMoved() )
	{
		if( mPlanePatch )
		{
			Vec3f pickOrigin = mGame->inputManager()->pickOrigin();
			Vec3f pickDir = mGame->inputManager()->pickDir();
			Vec3f planeHitPos;

			if( GameMath::rayInPlane( Ray( pickOrigin, pickDir ), mPlanePatch->center, mPlanePatch->baseNormal, planeHitPos ) )
			{
				Vec3i newLoc = mGame->world()->locFromPos( planeHitPos.minus( mPlanePatch->baseNormal ) );
				mGame->world()->addBlock( newLoc, mCurrMaterial, mPlanePatch->dir );
			}
		}
	}
	// x to remove patches.
	else if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_x ) ||
		( mGame->inputManager()->keyHeld( SDLK_x ) && mGame->inputManager()->mouseMoved() ) )
	{

		// Vec3f pickOrigin = mGame->inputManager()->pickOrigin();
		// Vec3f pickDir = mGame->inputManager()->pickDir();
		// if( !mGame->world()->lineOfSight( pickOrigin, pickOrigin.plus( pickDir.times( 9999.0f) ) ) )
		// {
		// 	printf("obstructed.\n");
		// }
		// else
		// {
		// 	printf("clear.\n");
		// }

		Patch *p = mGame->world()->getPatchAt( mGame->inputManager()->scenePos() );
		if( p )
		{
			mGame->world()->removePatch( p );
		}
	}
}

// ----------------------------------------------------------------------------
void EditingScene::allowUpdateMaterial()
{	
	InputEvent e;
	// *** mouseclick or mousemove while held "paints" material onto patches
	if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_u ) ||
		( mGame->inputManager()->keyHeld( SDLK_u ) && mGame->inputManager()->mouseMoved() ) )
	{
		Patch *p = mGame->world()->getPatchAt( mGame->inputManager()->scenePos() );
		if( p != NULL )
		{
			p->setMaterial( mCurrMaterial );
		}
	}
}

// ----------------------------------------------------------------------------
void EditingScene::allowCycleCurrentMaterial()
{
	InputEvent e;
	if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_1 ) )
	{		
		--mCurrMaterial;
	}	
	if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_2 ) )
	{		
		++mCurrMaterial;
	}

	mCurrMaterial = GameMath::clampi( mCurrMaterial, Material::NONE, Material::NUM_MATERIALS - 1 );
}

// ----------------------------------------------------------------------------
void EditingScene::allowEnterConsole()
{
	InputEvent e;
	if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_RETURN ) )
	{		
		mFSM.transitionTo( CONSOLE_MODE );
	}
}

// ----------------------------------------------------------------------------
void EditingScene::allowAdjustCamera( double dt )
{
	bool updated = false;

	// *** reset camera? ***
	InputEvent e;
	if( mGame->inputManager()->handle( e, InputManager::KEY_DOWN, SDLK_r ) )
	{
		mCamera.reset();
		updated = true;
	}
	else
	{
		// *** rotate camera? ***
		float yaw = 0.0f;
		float pitch = 0.0f;
		if( mGame->inputManager()->keyHeld( SDLK_LSHIFT ))
		{
			yaw = mGame->inputManager()->mouseDX() * CAMERA_TURN_SPEED * dt;
			pitch = -1.0f * mGame->inputManager()->mouseDY() * CAMERA_TURN_SPEED * dt;
			mCamera.rotateBy( yaw, pitch );	
			updated |= (yaw != 0.0f || pitch != 0.0f );
		}

		// **** move camera? ***
		float moveFront = 0.0f;
		float moveSide = 0.0f;
		float moveUp = 0.0f;

		if( mGame->inputManager()->keyHeld( SDLK_w ) )
		{
			moveFront += 1.0f;
		}
		if( mGame->inputManager()->keyHeld( SDLK_s ) )
		{
			moveFront -= 1.0f;
		}
		if( mGame->inputManager()->keyHeld( SDLK_a ) )
		{
			moveSide -= 1.0f;
		}
		if( mGame->inputManager()->keyHeld( SDLK_d ) )
		{
			moveSide += 1.0f;
		}
		if( mGame->inputManager()->keyHeld( SDLK_q ) )
		{
			moveUp += 1.0f;
		}
		if( mGame->inputManager()->keyHeld( SDLK_e ) )
		{
			moveUp -= 1.0f;
		}

		mCamera.moveBy( moveFront * CAMERA_MOVE_SPEED * dt, 
						moveSide * CAMERA_MOVE_SPEED * dt,
						moveUp * CAMERA_MOVE_SPEED * dt );
		updated |= (moveFront != 0.0f || moveSide != 0.0f || moveUp != 0.0f );
	}

	if( updated )
	{
		mCamera.apply();
	}
}

// ----------------------------------------------------------------------------
void EditingScene::tick( double dt )
{
	mFSM.tick( dt );
}

// ----------------------------------------------------------------------------
void EditingScene::showEditDetails()
{
	std::ostringstream sstream;
	sstream.precision(4);
	std::string out;

	// // *** left section - camera data ***
	sstream.str("");
	sstream << "Cam Pos: " << mCamera.pos().x << "   " << mCamera.pos().y << "   " << mCamera.pos().z;
	out = sstream.str();
	mGame->renderManager()->renderText( out.c_str(), 200, 50 );

	sstream.str("");
	sstream << "Angles: " << mCamera.yaw() << "   " << mCamera.pitch();
	out = sstream.str();
	mGame->renderManager()->renderText( out.c_str(), 200, 65 );



	// // *** middle section - patch and material data ***
	Patch *p = mGame->world()->getPatchAt( mGame->inputManager()->scenePos() );
	Material * mat = mGame->materialManager()->material( mCurrMaterial );
	
	Vec3f scenePos = mGame->inputManager()->scenePos();

	sstream.str("");
	sstream << "Active Material: " << mat->name;
	out = sstream.str();
	mGame->renderManager()->renderText( out.c_str(), 500, 50 );

	if( p != NULL )
	{
		Vec3i loc = mGame->world()->patchLoc( p );

		sstream.str("");
		sstream << "Loc: " << loc.i << "   " << loc.j << "   " << loc.k;
		out = sstream.str();
		mGame->renderManager()->renderText( out.c_str(), 500, 65 );

		sstream.str("");
		sstream << "Pos: " << scenePos.x << "   " << scenePos.y << "   " << scenePos.z;
		out = sstream.str();
		mGame->renderManager()->renderText( out.c_str(), 500, 80 );

		sstream.str("");
		sstream << "baseNormal: " << p->baseNormal.x << "   " << p->baseNormal.y << "   " << p->baseNormal.z;
		out = sstream.str();
		mGame->renderManager()->renderText( out.c_str(), 500, 95 );

		sstream.str("");
		sstream << "normal: " << p->normal.x << "   " << p->normal.y << "   " << p->normal.z;
		out = sstream.str();
		mGame->renderManager()->renderText( out.c_str(), 500, 110 );

		sstream.str("");
		sstream << "ao: " << p->ao;
		out = sstream.str();
		mGame->renderManager()->renderText( out.c_str(), 500, 125 );

		sstream.str("");
		sstream << "color: " << p->color.x << "   " << p->color.y << "   " << p->color.z;
		out = sstream.str();
		mGame->renderManager()->renderText( out.c_str(), 500, 140 );

		sstream.str("");
		sstream << "finalColor: " << p->finalColor.x << "   " << p->finalColor.y << "   " << p->finalColor.z;
		out = sstream.str();
		mGame->renderManager()->renderText( out.c_str(), 500, 155 );
	}


	// // *** right section - lighting data ***
	Light *l = mGame->world()->getLight( mCurrLight );

	sstream.str("");
	sstream << "Active Light: " << mCurrLight;
	out = sstream.str();
	mGame->renderManager()->renderText( out.c_str(), 800, 50 );

	sstream.str("");
	if( l->enabled )
	{
		sstream << "ON ";
	}
	else
	{
		sstream << "off";
	}
	out = sstream.str();
	mGame->renderManager()->renderText( out.c_str(), 800, 65 );


	sstream.str("");
	if( l->type == Light::AMBIENT )
	{
		sstream << "type: AMBIENT";
	}
	else if( l->type == Light::POINT)
	{
		sstream << "type: POINT";
	}
	else if( l->type == Light::DIRECTIONAL )
	{
		sstream << "type: DIRECTIONAL";
	} 
	out = sstream.str();
	mGame->renderManager()->renderText( out.c_str(), 800, 95 );


	sstream.str("");
	sstream << "pos: " << l->pos.x << "   " << l->pos.y << "   " << l->pos.z;
	out = sstream.str();
	mGame->renderManager()->renderText( out.c_str(), 800, 110 );

	if( l->type == Light::DIRECTIONAL )
	{
		Vec3f dir = l->end.minus( l->pos );
		dir.normalize();

		sstream.str("");
		sstream << "dir: " << dir.x << "   " << dir.y << "   " << dir.z;
		out = sstream.str();
		mGame->renderManager()->renderText( out.c_str(), 800, 125 );
	} 	

	sstream.str("");
	sstream << "rgb: " << l->color.x << "   " << l->color.y << "   " << l->color.z;
	out = sstream.str();
	mGame->renderManager()->renderText( out.c_str(), 800, 155 );

	hsv h = Vec3f::rgbToHSV( l->color );
	sstream.str("");
	sstream << "hsv: " << h.h << " " << h.s << " " << h.v;
	out = sstream.str();
	mGame->renderManager()->renderText( out.c_str(), 800, 170 );

	if( l->type == Light::POINT )
	{
		sstream.precision(9);
		sstream.str("");
		sstream << "intensity: " << l->intensity;
		out = sstream.str();
		mGame->renderManager()->renderText( out.c_str(), 800, 185 );		
	}
}

// ----------------------------------------------------------------------------
void EditingScene::showBackdropDetails()
{
	std::ostringstream sstream;
	sstream.precision(4);
	std::string out;

	// // *** left section - camera data ***
	sstream.str("");
	sstream << "Backdrop Stops:";
	out = sstream.str();
	mGame->renderManager()->renderText( out.c_str(), 200, 50 );

	int y = 80;
	for( int i=0; i < mGame->backdrop().stopCount(); ++i )
	{
		BackdropStop& stop = mGame->backdrop().stop(i);

		sstream.str("");
		sstream << stop.percent << "      " << stop.color.x << "       " << stop.color.y << "        " << stop.color.z;
		out = sstream.str();
		mGame->renderManager()->renderText( out.c_str(), 200, y );	
		y += 15;	
	}
}

// ----------------------------------------------------------------------------
void EditingScene::render()
{
	if( mFSM.state() == BACKDROP_MODE )
	{
		showBackdropDetails();
	}
	else
	{
		showEditDetails();
	}

    // draw world axes for reference
	mGame->renderManager()->renderLine( Vec3f::ZERO, Vec3f::RED, Vec3f( 5000.0f, 0.0f, 0.0f ), mGame->renderManager()->clearColor() );
    mGame->renderManager()->renderLine( Vec3f::ZERO, Vec3f::GREEN, Vec3f( 0.0f, 5000.0f, 0.0f ), mGame->renderManager()->clearColor() );
    mGame->renderManager()->renderLine( Vec3f::ZERO, Vec3f::BLUE, Vec3f( 0.0f, 0.0f, 5000.0f ), mGame->renderManager()->clearColor() );

}
