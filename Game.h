//
// Created by zthacker on 4/5/2024.
//

#ifndef LASERLIBRE_GAME_H
#define LASERLIBRE_GAME_H

#include "common.h"
#include "Entity.h"
#include <iostream>

using namespace std;

enum {
    LASER_STATE_SETUP,
    LASER_STATE_PLAYING,
    LASER_STATE_PAUSED,
    LASER_STATE_GAME_OVER
};

class Game {
public:
    Game();
    ~Game();
    void prepareScene();
    void presentScene();
    void doInput();
    void logic();
    void draw();



private:

    //Inits
    void initializeSDL();
    void initializeGame();

    //Draw
    void blit(SDL_Texture* texture, int x, int y);
    SDL_Texture* loadTexture(const string &filepath);

    //Input
    void keyPressUp(SDL_KeyboardEvent* event);
    void keyPressDown(SDL_KeyboardEvent* event);

    //Player
    void initializePlayer();
    Entity* player{};

    Entity* fighterHead{};
    Entity* fighterTail{};
    Entity* bulletHead{};
    Entity* bulletTail{};

    SDL_Window* m_window{};
    SDL_Renderer* m_renderer{};

    int m_state;
    int keyboard[MAX_KEYBOARD_KEYS]{};
};


#endif //LASERLIBRE_GAME_H
