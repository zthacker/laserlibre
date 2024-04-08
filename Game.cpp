//
// Created by zthacker on 4/5/2024.
//

#include <algorithm>
#include "Game.h"

Game::Game() {
    m_state = LASER_STATE_SETUP;
    initializeSDL();
    inititalizeTextures();
    initializePlayer();
    initializeEnemy();
}

Game::~Game() {
    IMG_Quit();
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_DestroyTexture(m_bulletTexture);
    SDL_DestroyTexture(m_playerTexture);
    SDL_DestroyTexture(m_enemyTexture);
    SDL_Quit();
    m_state = LASER_STATE_SETUP;
}

void Game::initializeSDL() {
    int rendererFlags, windowFlags;
    rendererFlags = SDL_RENDERER_ACCELERATED;
    windowFlags = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    m_window = SDL_CreateWindow("Some Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    m_renderer = SDL_CreateRenderer(m_window, -1, rendererFlags);

    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
}

void Game::inititalizeTextures() {
    m_playerTexture = loadTexture(R"(C:\Users\zthacker\laserlibre\gfx\player.png)");
    m_bulletTexture = loadTexture(R"(C:\Users\zthacker\laserlibre\gfx\playerBullet.png)");
    m_enemyTexture = loadTexture(R"(C:\Users\zthacker\laserlibre\gfx\enemy.png)");
}

void Game::initializePlayer() {
    this->m_player = new Entity(100, 100, m_playerTexture);
    SDL_QueryTexture(m_player->texture, nullptr, nullptr, &m_player->w, &m_player->h);

    this->m_player->reload = 0;
    this->m_player->health = 1;
    this->m_player->side = SIDE_PLAYER;
    this->m_player->id = rand();
    this->m_fighters.push_back(m_player);
}

void Game::initializeEnemy() {
    this->enemySpawnTimer = 0;
}

void Game::prepareScene() {
    SDL_SetRenderDrawColor(m_renderer, 96, 128, 255, 255);
    SDL_RenderClear(m_renderer);
}

void Game::presentScene() {
    SDL_RenderPresent(m_renderer);
}

void Game::blit(SDL_Texture *texture, int x, int y) {
    SDL_Rect dest;

    dest.x = x;
    dest.y = y;
    SDL_QueryTexture(texture, nullptr, nullptr, &dest.w, &dest.h);
    SDL_RenderCopy(m_renderer, texture, nullptr, &dest);
}


SDL_Texture* Game::loadTexture(const string &filepath) {
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filepath.c_str());
    return IMG_LoadTexture(m_renderer,filepath.c_str());
}

void Game::keyPressUp(SDL_KeyboardEvent *event) {
    if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS)
    {
        m_keyboard[event->keysym.scancode] = 0;
    }
}

void Game::keyPressDown(SDL_KeyboardEvent *event) {
    if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS)
    {
        m_keyboard[event->keysym.scancode] = 1;
    }
}

void Game::doInput() {
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                exit(0);
                break;

            case SDL_KEYDOWN:
                keyPressDown(&event.key);
                break;

            case SDL_KEYUP:
                keyPressUp(&event.key);
                break;

            default:
                break;
        }
    }
}

void Game::logic() {
    doPlayer();
    doFighters();
    doBullets();
    spawnEnemies();

}

void Game::doPlayer() {
    m_player->dx = m_player->dy = 0;

    if (m_player->reload > 0)
    {
        m_player->reload--;
    }

    if (m_keyboard[SDL_SCANCODE_UP])
    {
        m_player->dy = -PLAYER_SPEED;
    }

    if (m_keyboard[SDL_SCANCODE_DOWN])
    {
        m_player->dy = PLAYER_SPEED;
    }

    if (m_keyboard[SDL_SCANCODE_LEFT])
    {
        m_player->dx = -PLAYER_SPEED;
    }

    if (m_keyboard[SDL_SCANCODE_RIGHT])
    {
        m_player->dx = PLAYER_SPEED;
    }

    if (m_keyboard[SDL_SCANCODE_E] && m_player->reload == 0)
    {
        fireBullet();
    }

    m_player->x += m_player->dx;
    m_player->y += m_player->dy;
}

void Game::doBullets() {

    list<Entity*> removeList;

    for(Entity* b : m_bullets) {
        b->x += b->dx;
        b->y += b->dy;
        if(bulletHitFighter(b) || b->x > SCREEN_WIDTH || b->health == 0) {
            removeList.push_back(b);
        }
    }

    for(Entity* e : removeList) {
        m_bullets.remove(e);
        delete e;
    }
}

void Game::doFighters() {

    list<Entity*> removeList;

    for(Entity* f : m_fighters) {
        f->x += f->dx;
        f->y += f->dy;
        if(f != m_player && (f->x < -f->w) || f->health == 0) {
            removeList.push_back(f);
        }
    }

    for(Entity* e : removeList) {
        m_fighters.remove(e);
        delete e;
    }
}

void Game::spawnEnemies() {

    if(--enemySpawnTimer <= 0) {
        auto* enemy = new Entity(SCREEN_WIDTH,rand()%SCREEN_HEIGHT,m_enemyTexture);
        SDL_QueryTexture(enemy->texture, nullptr, nullptr, &enemy->w, &enemy->h);
        enemy->dx = -(2 + (rand() % 4));
        enemy->health = 1;
        enemy->side = SIDE_ALIEN;
        enemy->id = rand();
        m_fighters.push_back(enemy);

        enemySpawnTimer = 30 + (rand() % 60);
    }
}

void Game::draw() {
    drawPlayer();
    drawBullets();
    drawFighters();
}

void Game::drawPlayer() {
    blit(m_player->texture, m_player->x, m_player->y);
}

void Game::drawBullets() {
    for(Entity* b : m_bullets) {
        blit(b->texture, b->x, b->y);
    }
}

void Game::drawFighters() {
    for(const auto& f : m_fighters) {
        blit(f->texture, f->x, f->y);
    }
}

void Game::fireBullet() {
    auto* bullet = new Entity(m_player->x, m_player->y, m_bulletTexture);
    bullet->dx = PLAYER_BULLET_SPEED;
    bullet->health = 1;
    SDL_QueryTexture(bullet->texture, nullptr, nullptr, &bullet->w, &bullet->h);
    bullet->y += (m_player->h /2) - (bullet->h / 2);
    bullet->health = 1;
    bullet->side = SIDE_PLAYER;
    bullet->id = rand();
    m_bullets.push_back(bullet);

    m_player->reload = 8;
}

int Game::collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    return (max(x1, x2) < min(x1 + w1, x2 + w2)) && (max(y1, y2) < min(y1 + h1, y2 + h2));
}

int Game::bulletHitFighter(Entity *b) {
    for(auto it = m_fighters.begin(); it!=m_fighters.end();) {
        Entity* f = *it;
        if(f->side != b->side && collision(b->x, b->y, b->w, b->h, f->x, f->y, f->w, f->h)) {
            b->health = 0;
            f->health = 0;
            return 1;
        }
        it++;
    }

    return 0;
}












