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

typedef struct {
    int x;
    int y;
    int speed;
} Star;

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
    void initializeStarfield();
    void initializeGame();

    //Draw
    void blit(SDL_Texture* texture, int x, int y);
    void blitRect(SDL_Texture* texture, SDL_Rect* src, int x, int y);
    SDL_Texture* loadTexture(const string &filepath);
    void drawBackground();
    void drawStarfield();
    void drawBullets();
    void drawFighters();
    void drawDebris();
    void drawExplosions();

    //Input
    void keyPressUp(SDL_KeyboardEvent* event);
    void keyPressDown(SDL_KeyboardEvent* event);

    //Logic
    void doBackground();
    void doStarfield();
    void doPlayer();
    void doEnemies();
    void doBullets();
    void doFighters();
    void spawnEnemies();
    void clipPlayer();
    void doExplosions();
    void doDebris();

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
    SDL_Texture* m_background{};
    SDL_Texture* m_explosionTexture{};
    list<Entity*> m_explosions{};
    list<Entity*> m_debris{};
    void addExplosions(int x, int y, int num);
    void addDebris(Entity* e);

    int m_state;
    int m_keyboard[MAX_KEYBOARD_KEYS]{};
    Star m_stars[MAX_STARS];
    int m_gameResetTimer;
    int m_backgroundX;


};


#endif //LASERLIBRE_GAME_H
