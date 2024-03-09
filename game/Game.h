#pragma once

#include <stdlib.h>
#include <vector>
#include "../fsm/FSM.h"
#include "Backdrop.h"
#include "Fader.h"

class InputManager;
class RenderManager;
class SoundManager;
class ResourceManager;
class MaterialManager; 
class PlayingScene;
class EditingScene;
class Scene;
class World;

class Game
{
private:
	bool               mDev;
	bool               mQuit;
	InputManager     * mInputManager = NULL;
	RenderManager    * mRenderManager = NULL;
    SoundManager     * mSoundManager = NULL;
	ResourceManager  * mResourceManager = NULL;
    MaterialManager  * mMaterialManager = NULL;

    std::vector<Scene *> mScenes;
    PlayingScene     * mPlayingScene = NULL;
    EditingScene     * mEditingScene = NULL;
    World            * mWorld = NULL;
    Backdrop 		   mBackdrop;
    Fader              mFader;

    FSM<Game>          mFSM;

    void whileEditing( double dt );
    void whilePlaying( double dt );

public:
	static const int EDITING_STATE = 1;
	static const int PLAYING_STATE = 2;

	bool             quit() { return mQuit; }
	InputManager*    inputManager() { return mInputManager; }
    RenderManager*   renderManager() { return mRenderManager; }
    SoundManager*    soundManager()  { return mSoundManager; }
    ResourceManager* resourceManager() { return mResourceManager; }
    MaterialManager* materialManager() { return mMaterialManager; }

    Fader    &       fader()    { return mFader; }
    Backdrop &       backdrop() { return mBackdrop; }
    World        *   world() { return mWorld; }
    EditingScene *   editingScene() { return mEditingScene; }
    PlayingScene *   playingScene() { return mPlayingScene; }

	// ----
	Game();
	void init( bool dev = false );
	void shutdown();

    void runCommand( const std::string& command );
	void tick( double dt );
};