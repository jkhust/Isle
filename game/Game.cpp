#include "Game.h"

#include <SDL.h>
#include <SDL_image.h>

#include "../input/InputManager.h"
#include "../render/RenderManager.h"
#include "../sound/SoundManager.h"
#include "../resource/ResourceManager.h"
#include "../map/MaterialManager.h"

#include "../scene/Scene.h"
#include "../scene/playing/PlayingScene.h"
#include "../scene/editing/EditingScene.h"

#include "../world/World.h"
#include "../world/Patch.h"
#include "../world/Light.h"

#include "../util/GameMath.h"
#include "../util/StringUtil.h"
#include "../util/Vec3i.h"
#include "../util/Vec3f.h"

#include "Constants.h"

#include <fstream>
#include <cstddef>        // std::size_t

// ----------------------------------------------------------------
Game::Game()
{
}

// ----------------------------------------------------------------
void Game::init( bool dev )
{
    SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2
    IMG_Init( IMG_INIT_JPG | IMG_INIT_PNG );

	mDev = dev;
	mQuit = false;

	mInputManager = new InputManager();
	mInputManager->init( this );

	mRenderManager = new RenderManager();
	mRenderManager->init( this );

	mSoundManager = new SoundManager();
	mSoundManager->init( this );
	mSoundManager->loadSound("move_accepted", "assets/sounds/move_accepted.wav");
	mSoundManager->loadSound("move_rejected", "assets/sounds/move_rejected.wav");
	//TODO: fix these
	//mSoundManager->loadSound("moves_cleared", "assets/sounds/moves_cleared.wav");
	//mSoundManager->loadSound("void_success", "assets/sounds/void_success.wav");

	mMaterialManager = new MaterialManager();
	mMaterialManager->init( this );

	mResourceManager = new ResourceManager();
	mResourceManager->init( this );	
	mResourceManager->loadTexture( OCEAN_TEXTURE, "assets/sand8.png" );
	mResourceManager->loadTexture( GRASS_TEXTURE, "assets/sand8.png" );
	mResourceManager->loadTexture( DIRT_TEXTURE, "assets/sand8.png" );
	mResourceManager->loadTexture( SAND_TEXTURE, "assets/sand8.png" );

	// *** setup scenes ***
	mEditingScene = new EditingScene( this );
	mPlayingScene = new PlayingScene( this );
	mScenes.push_back( mEditingScene );
	mScenes.push_back( mPlayingScene );

	mWorld = new World( this, Vec3f( -World::WORLD_EXTENT, -World::WORLD_EXTENT , -World::WORLD_EXTENT ) ,
		Vec3f( World::WORLD_EXTENT, World::WORLD_EXTENT , World::WORLD_EXTENT ) );

	// *** load some test data ***
	for( int i=1; i < 2; ++i )
	{
		for( int k=1; k < 2; ++k )
		{
			mWorld->addBlock( Vec3i( i, 0, k ), Material::OCEAN );
		}
	}

	Light *l = mWorld->getLight(0);
	l->enabled = true;
	l->type = Light::AMBIENT;
	l->color.set( 0.2f, 0.2f, 0.2f );

	mBackdrop.init( this );
	mBackdrop.add( 0.0f, Vec3f::RED );
	mBackdrop.add( 0.3f, Vec3f::VIOLET );
	mBackdrop.add( 1.0f, Vec3f::BLUE );

	mFader.init( this );
	mFader.fade( Vec3f( 1.0f, 1.0f, 1.0f), 1.0f, 0.0f, 1.0f );

	mFSM.init( this );
	mFSM.addState( EDITING_STATE, &Game::whileEditing );
	mFSM.addState( PLAYING_STATE, &Game::whilePlaying );
	mFSM.addTransition( EDITING_STATE, PLAYING_STATE );
	mFSM.addTransition( PLAYING_STATE, EDITING_STATE );

	mEditingScene->activate();
	mPlayingScene->deactivate();
}

// ----------------------------------------------------------------
void Game::tick( double dt )
{
	mInputManager->tick( dt );
	mQuit |= mInputManager->quit();

	mResourceManager->tick( dt );

	mFSM.tick( dt );

	mRenderManager->beginFrame();
		if( mBackdrop.visible() )
		{
			mBackdrop.render();	
		}
	
		// *** tick every scene that's not paused ***
		for( int i=0; i < mScenes.size(); ++i )
		{
			if( mScenes[i]->isPlaying() )
			{
				mScenes[i]->tick( dt );
			}
		}

		if( !mWorld->paused() )
		{
			mWorld->tick( dt );		
		}

		// ***  then render every scene that has its camera enabled ***
		for( int i=0; i < mScenes.size(); ++i )
		{
			if( mScenes[i]->isActive() )
			{
				mScenes[i]->render();
				mWorld->render( mScenes[i]->camera() );  
			}
		}

		mFader.tick( dt );
		mFader.render();

	mRenderManager->endFrame();

	if( mDev )
	{
		mInputManager->debug();
	}

    // once done gathering keys, see if we should quit.
    InputEvent e;
    if( mInputManager->handle( e, InputManager::KEY_DOWN, SDLK_ESCAPE ) )
    {
    	mQuit = true;
    }
}

// ----------------------------------------------------------------
void Game::whileEditing( double dt )
{
	InputEvent e;
	if( mInputManager->handle( e, InputManager::KEY_DOWN, SDLK_TAB ) )
	{
		mEditingScene->deactivate();
		mPlayingScene->activate();
		mFSM.transitionTo( PLAYING_STATE );
	}	
}

// ----------------------------------------------------------------
void Game::whilePlaying( double dt )
{
	InputEvent e;
	if( mInputManager->handle( e, InputManager::KEY_DOWN, SDLK_TAB ) )
	{
		mPlayingScene->deactivate();
		mEditingScene->activate();	
		mFSM.transitionTo( EDITING_STATE );
	}	
}

// ----------------------------------------------------------------
void Game::shutdown()
{
	mResourceManager->shutdown();
	mSoundManager->shutdown();
	mRenderManager->shutdown();
	mInputManager->shutdown();

	SDL_Quit();
}

// ----------------------------------------------------------------
void Game::runCommand( const std::string& command )
{
	if( command.length() == 0 ) return;

	std::vector<std::string> parts = StringUtil::tokenize( command );

	if( false )
	{

	}
	// shortcut for exec assets/<map>.txt
	else if( parts.size() == 1 && StringUtil::beginsWith( parts[0], "map" ) )
	{	
		std::string execCommand = "exec assets/" + parts[0] + ".txt";
		runCommand( execCommand );

		mFader.fade( Vec3f( 1.0f, 1.0f, 1.0f), 1.0f, 0.0f, 1.0f );
	}
	else if( parts.size() == 2 && parts[0] == "saveas" )
	{
		mWorld->writeToFile( parts[1].c_str() );
	}
	else if( parts.size() == 2 && parts[0] == "exec" )
	{
		std::ifstream fin( parts[1] );
		std::string line;
		while( std::getline( fin, line ) )
		{
			runCommand( line );
		}
		fin.close();
	}
	else if (parts.size() == 1 && parts[0] == "clear" )
	{
		mWorld->clear();
	}
	else if (parts.size() == 1 && parts[0] == "backdropclear" )
	{
		mBackdrop.clear();
	}
	else if (parts.size() == 1 && parts[0] == "resetao" )
	{
		for( int i=0; i < mWorld->patches().size(); ++i )
		{
			mWorld->patches()[i]->resetAO();
		}
	}
	else if (parts.size() == 2 && parts[0] == "pause" )
	{
		int val = atoi( parts[1].c_str() );
		mWorld->setPaused( (val == 1 ) );
	}
	else if( parts.size() == 2 && parts[0] == "backdrop" )
	{
		int val = atoi( parts[1].c_str() );
		mBackdrop.setVisible( ( val == 1 ) );
	}
	else if( parts.size() == 2 && parts[0] == "backdropedit" )
	{
		int index = atoi( parts[1].c_str() );
		mEditingScene->beginEditingBackdrop( index );
	}
	else if( parts.size() == 2 && parts[0] == "showbasenormals" )
	{
		int val = atoi( parts[1].c_str() );
		mWorld->showBaseNormals( (val == 1 ) );
	}
	else if( parts.size() == 2 && parts[0] == "shownormals" )
	{
		int val = atoi( parts[1].c_str() );
		mWorld->showNormals( (val == 1 ) );
	}
	else if( parts.size() == 2 && parts[0] == "fullbright" )
	{
		int val = atoi( parts[1].c_str() );
		mWorld->setFullbright( (val == 1 ) );
	}
	else if( parts.size() == 2 && parts[0] == "ambientlight" )
	{
		int val = atoi( parts[1].c_str() );
		mWorld->enableAmbientLight( (val == 1 ) );
	}
	else if( parts.size() == 2 && parts[0] == "pointlights" )
	{
		int val = atoi( parts[1].c_str() );
		mWorld->enablePointLights( (val == 1 ) );
	}
	else if( parts.size() == 2 && parts[0] == "dirlights" )
	{
		int val = atoi( parts[1].c_str() );
		mWorld->enableDirLights( (val == 1 ) );
	}
	else if( parts.size() == 3 && parts[0] == "lightenabled" )
	{
		int index = atoi( parts[1].c_str() );
		int val = atoi( parts[2].c_str() );

		mWorld->getLight( index )->enabled = ( val == 1 );
	}
	else if( parts.size() == 2 && parts[0] == "aaenabled" )
	{
		int val = atoi( parts[1].c_str() );
		mRenderManager->setAntialiasingEnabled( (val == 1 ) );
	}
	else if( parts.size() == 5 && parts[0] == "backdropadd" )
	{
		float percent = atof( parts[1].c_str() );
		float r = atof( parts[2].c_str() );
		float g = atof( parts[3].c_str() );
		float b = atof( parts[4].c_str() );

		// int color = (int) strtol( parts[2].c_str(), NULL, 0 );
		// unsigned char r = ((color >> 16) & 0xFF);
		// unsigned char g = ((color >> 8) & 0xFF);
		// unsigned char b = (color & 0xFF);
		mBackdrop.add( percent, Vec3f( r, g, b ) );
	}
	else if( parts.size() == 3 && parts[0] == "lighttype" )
	{
		int index = atoi( parts[1].c_str() );
		int val = atoi( parts[2].c_str() );

		mWorld->getLight( index )->type = val;
	}
	else if( parts.size() == 3 && parts[0] == "lightintensity" )
	{
		int index = atoi( parts[1].c_str() );
		float intensity = atof( parts[2].c_str() );

		mWorld->getLight( index )->intensity = intensity;
	}
	else if( parts.size() == 4 && parts[0] == "addblock" )
	{
		int i = atoi( parts[1].c_str() );
		int j = atoi( parts[2].c_str() );
		int k = atoi( parts[3].c_str() );
		mWorld->addBlock( Vec3i( i, j, k ), mEditingScene->currMaterial() );	
	}
	else if( parts.size() == 5 && parts[0] == "lightrgb" )
	{
		int index = atoi( parts[1].c_str() );

		float r = atof( parts[2].c_str() );
		float g = atof( parts[3].c_str() );
		float b = atof( parts[4].c_str() );

		mWorld->getLight( index )->color.set( r, g, b );

		// *** TODO: repurpose for hex later ***
		// int color = (int) strtol( parts[2].c_str(), NULL, 0 );
		// unsigned char r = ((color >> 16) & 0xFF);
		// unsigned char g = ((color >> 8) & 0xFF);
		// unsigned char b = (color & 0xFF);
		//mWorld->getLight( index )->color.setBytes( r, g, b);
	}
	else if( parts.size() == 5 && parts[0] == "lighthsv" )
	{
		int index = atoi( parts[1].c_str() );

		hsv h;
		h.h = atof( parts[2].c_str() );
		h.s = atof( parts[3].c_str() );
		h.v = atof( parts[4].c_str() );

		Vec3f rgb = Vec3f::hsvToRGB( h );

		mWorld->getLight( index )->color.set( rgb );
	}
	else if( parts.size() == 5 && parts[0] == "lightpos" )
	{
		int index = atoi( parts[1].c_str() );

		float x = atof( parts[2].c_str() );
		float y = atof( parts[3].c_str() );
		float z = atof( parts[4].c_str() );
		
		mWorld->getLight( index )->pos.set( x, y, z );
	}
	else if( parts.size() == 5 && parts[0] == "lightend" )
	{
		int index = atoi( parts[1].c_str() );

		float x = atof( parts[2].c_str() );
		float y = atof( parts[3].c_str() );
		float z = atof( parts[4].c_str() );
		
		mWorld->getLight( index )->end.set( x, y, z );
	}
	else if( parts.size() == 6 && parts[0] == "editcam" )
	{
		float x = atof( parts[1].c_str() );
		float y = atof( parts[2].c_str() );
		float z = atof( parts[3].c_str() );
		float yaw = atof( parts[4].c_str() );
		float pitch = atof( parts[5].c_str() );

		mEditingScene->camera().moveTo( x, y, z );
		mEditingScene->camera().rotateTo( yaw, pitch );

		if( mEditingScene->isPlaying() )
		{
			mEditingScene->camera().apply();
		}
	}
	else if( parts.size() == 7 && parts[0] == "playcam" )
	{
		float x = atof( parts[1].c_str() );
		float y = atof( parts[2].c_str() );
		float z = atof( parts[3].c_str() );
		float yaw = atof( parts[4].c_str() );
		float pitch = atof( parts[5].c_str() );
		float zoom = atof( parts[6].c_str() );

		mPlayingScene->camera().moveTo( x, y, z );
		mPlayingScene->camera().rotateTo( yaw, pitch );
		mPlayingScene->camera().zoomTo( zoom );

		if( mPlayingScene->isPlaying() )
		{
			mPlayingScene->camera().apply();
		}
	}
	else if( parts.size() == 14 && parts[0] == "addpatch")
	{
		float ax = atof( parts[1].c_str() );
		float ay = atof( parts[2].c_str() );
		float az = atof( parts[3].c_str() );

		float bx = atof( parts[4].c_str() );
		float by = atof( parts[5].c_str() );
		float bz = atof( parts[6].c_str() );

		float cx = atof( parts[7].c_str() );
		float cy = atof( parts[8].c_str() );
		float cz = atof( parts[9].c_str() );

		float baseCenterX = atof( parts[10].c_str() );
		float baseCenterY = atof( parts[11].c_str() );
		float baseCenterZ = atof( parts[12].c_str() );

		int mat = atoi( parts[13].c_str() );

		mWorld->addPatch( new Patch( mWorld, Vec3f( ax, ay, az ), Vec3f( bx, by, bz ), Vec3f( cx, cy, cz ), Vec3f( baseCenterX, baseCenterY, baseCenterZ ), mat ) );
	}
	else
	{
		printf("Ignored command:  %s\n", command.c_str() );
	}
}