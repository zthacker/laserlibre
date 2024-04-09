//
// Created by zthacker on 4/5/2024.
//

#ifndef LASERLIBRE_GAME_H
#define LASERLIBRE_GAME_H

#include "common.h"
#include "Entity.h"
#include <list>
#include <iostream>
#include <memory>

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
    void drawBullets();
    void drawFighters();

    //Input
    void keyPressUp(SDL_KeyboardEvent* event);
    void keyPressDown(SDL_KeyboardEvent* event);

    //Logic
    void doPlayer();
    void doEnemies();
    void doBullets();
    void doFighters();
    void spawnEnemies();
    void clipPlayer();

    //Player
    Entity* m_player{};
    SDL_Texture* m_playerTexture{};

    //Bullet
    void fireBullet();
    SDL_Texture* m_playerBulletTexture{};
    int bulletHitFighter(Entity* b);

    //Enemy
    void fireEnemeyBullet(Entity* e);
    SDL_Texture* m_enemyTexture{};
    SDL_Texture* m_enemyBulletTexture{};
    int enemySpawnTimer{};


    //Entities
    list<Entity*> m_fighters{};
    list<Entity*> m_bullets{};

    //SDL
    SDL_Window* m_window{};
    SDL_Renderer* m_renderer{};

    //Collision
    int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

    //Enemy Bullet
    void calculateSlope(int x1, int y1, int x2, int y2, float *dx, float *dy);

    //Game
    void resetGame();

    int m_state;
    int m_keyboard[MAX_KEYBOARD_KEYS]{};
    int m_gameResetTimer;
};


#endif //LASERLIBRE_GAME_H
