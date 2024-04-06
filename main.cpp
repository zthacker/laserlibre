#include <iostream>
#include "common.h"
#include "Game.h"

using namespace std;

static void capFrameRate(long *then, float *remainder);


int main(int argc, char *argv[]) {

    long  then;
    float remainder;

    Game* game = new Game();
    then = SDL_GetTicks();

    remainder = 0;

    while(1) {
        //prepare the game scene
        game->prepareScene();

        //do input functionality
        game->doInput();

        //run game logic
        game->logic();

        //run game drawing
        game->draw();

        //present the game scene
        game->presentScene();

        //cap the framerate
        capFrameRate(&then, &remainder);
    }

    return 0;
}

static void capFrameRate(long *then, float *remainder)
{
    long wait, frameTime;

    wait = 16 + *remainder;

    *remainder -= (int)*remainder;

    frameTime = SDL_GetTicks() - *then;

    wait -= frameTime;

    if (wait < 1)
    {
        wait = 1;
    }

    SDL_Delay(wait);

    *remainder += 0.667;

    *then = SDL_GetTicks();
}