#include "Scene.h"

#include "../game/Game.h"

// --------------------------------------------------
Scene::Scene( Game * game )
{ 
	mGame = game;
	deactivate();
}

// --------------------------------------------------
void Scene::activate()
{
	mActive = true;
	resume();
}

// --------------------------------------------------
void Scene::deactivate()
{
	mActive = false;
	pause();
}

// --------------------------------------------------
void Scene::pause()
{
	mPlaying = false;
}

// --------------------------------------------------
void Scene::resume()
{
	mPlaying = true;
}
