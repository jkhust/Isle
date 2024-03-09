#include <SDL.h>
#include <stdio.h>

#include "game/Game.h"

// ---------------------------------------------------------------------
int main(int argc, char* argv[])
{
    Game game;
    game.init( true );

    Uint32 lastTime = SDL_GetTicks();

    while( !game.quit() )
    {
        Uint32 now = SDL_GetTicks();

        if( now - lastTime > 0 )
        {
            double dt = (now - lastTime ) / 1000.0;
            lastTime = now;

            game.tick( dt );
        }
    }

    game.shutdown();
    return 0;
}

