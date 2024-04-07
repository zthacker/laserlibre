//
// Created by zthacker on 4/5/2024.
//

#ifndef LASERLIBRE_GAME_H
#define LASERLIBRE_GAME_H

#include "common.h"
#include "Entity.h"
#include <list>
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
    void inititalizeTextures();
    void initializePlayer();
    void initializeEnemy();
    void initializeGame();

    //Draw
    void blit(SDL_Texture* texture, int x, int y);
    SDL_Texture* loadTexture(const string &filepath);
    void drawPlayer();
    void drawBullets();
    void drawFighters();

    //Input
    void keyPressUp(SDL_KeyboardEvent* event);
    void keyPressDown(SDL_KeyboardEvent* event);

    //Logic
    void doPlayer();
    void doBullets();
    void doFighters();
    void spawnEnemies();

    //Player
    Entity* m_player{};
    SDL_Texture* m_playerTexture{};


    //Bullet
    void fireBullet();
    SDL_Texture* m_bulletTexture{};

    //Enemy
    SDL_Texture* m_enemyTexture;
    int enemySpawnTimer;

    //Entities
    list<Entity*> m_fighters{};
    list<Entity*> m_bullets{};

    //SDL
    SDL_Window* m_window{};
    SDL_Renderer* m_renderer{};

    int m_state;
    int m_keyboard[MAX_KEYBOARD_KEYS]{};
};


#endif //LASERLIBRE_GAME_H
