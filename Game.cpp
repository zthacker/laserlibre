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
    SDL_DestroyTexture(m_playerBulletTexture);
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
    m_playerBulletTexture = loadTexture(R"(C:\Users\zthacker\laserlibre\gfx\playerBullet.png)");
    m_enemyBulletTexture = loadTexture(R"(C:\Users\zthacker\laserlibre\gfx\alienBullet.png)");
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
    doEnemies();
    doFighters();
    doBullets();
    spawnEnemies();
    clipPlayer();

    if(m_player == nullptr && --m_gameResetTimer <= 0) {
        resetGame();
    }

}

void Game::doPlayer() {

    if(m_player != nullptr) {
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

}

void Game::doEnemies() {
    for(Entity* f : m_fighters) {
        if(f != m_player && m_player != nullptr && --f->reload <= 0) {
            fireEnemeyBullet(f);
        }
    }
}

void Game::doBullets() {

    list<Entity*> removeList;

    for(Entity* b : m_bullets) {
        b->x += b->dx;
        b->y += b->dy;

        //check if any of the bullets are leaving the screen
        if(bulletHitFighter(b) || b->x < -b->w || b->y < -b->h || b->x > SCREEN_WIDTH || b->y > SCREEN_HEIGHT || b->health == 0) {
            removeList.push_back(b);
        }
    }

    //remove the entities from the bullets list and delete them
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

        //if you're not the player and you're off the screen, your health is 0
        if(f != m_player && (f->x < -f->w)) {
            f->health = 0;
        }

        //if your health is 0 and you're the player, set to nullptr, or remove it;
        if(f->health == 0) {
            if(f == m_player) {
                m_player = nullptr;
            }
            removeList.push_back(f);
        }

    }

    //remove entities from the fighters list and delete them
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

        //don't want the enemies to fire right away -- this is a 1 to 2 second window before they can fire
        enemy->reload = FPS * (1 +(rand() %3));
        m_fighters.push_back(enemy);

        enemySpawnTimer = 30 + (rand() % 60);
    }
}

void Game::draw() {
    drawBullets();
    drawFighters();
}


void Game::drawBullets() {
    for(Entity* b : m_bullets) {
        if(b != nullptr) {
            blit(b->texture, b->x, b->y);
        }
    }
}

void Game::drawFighters() {
    for(const auto& f : m_fighters) {
        if(f != nullptr) {
            blit(f->texture, f->x, f->y);
        }
    }
}

void Game::fireBullet() {
    auto* bullet = new Entity(m_player->x, m_player->y, m_playerBulletTexture);
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

void Game::fireEnemeyBullet(Entity* e) {
    auto* bullet = new Entity(e->x, e->y, m_enemyBulletTexture);
    bullet->id = rand();
    bullet->health = 1;
    bullet->side = SIDE_ALIEN;
    SDL_QueryTexture(bullet->texture, nullptr, nullptr, &bullet->w, &bullet->h);

    //puts this in the middle of the entity's height and width
    bullet->x += (e->w / 2) - (bullet->w / 2);
    bullet->y += (e->h / 2) - (bullet->h / 2);

    //fire bullet from entity to player, getting the normalized steps of the slope to move
    calculateSlope(m_player->x + (m_player->w / 2), m_player->y + (m_player->h / 2), e->x, e->y, &bullet->dx, &bullet->dy);

    //create the delta of x and y for movement
    bullet->dx *= ALIEN_BULLET_SPEED;
    bullet->dy *= ALIEN_BULLET_SPEED;

    m_bullets.push_back(bullet);

    //fire anytime within the next 2 seconds (this'll be < 120, basically)
    e->reload = (rand() % FPS * 2);
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

void Game::calculateSlope(int x1, int y1, int x2, int y2, float *dx, float *dy) {
    int steps = max(abs(x1 - x2), abs(y1 - y2));

    if (steps == 0)
    {
        *dx = *dy = 0;
        return;
    }

    //dx or dy will be 1 or -1 always; meaning the Bulllet Speed assigned will be a constant speed along that axis

    *dx = (x1 - x2);
    *dx /= steps;

    *dy = (y1 - y2);
    *dy /= steps;
}

//make sure the player stays within the bounds of the screen
void Game::clipPlayer() {

    if(m_player != nullptr) {

        if(m_player->x < 0) {
            m_player-> x = 0;
        }

        if(m_player->y < 0) {
            m_player->y = 0;
        }

        if (m_player->x > SCREEN_WIDTH / 2)
        {
            m_player->x = SCREEN_WIDTH / 2;
        }

        if (m_player->y > SCREEN_HEIGHT - m_player->h)
        {
            m_player->y = SCREEN_HEIGHT - m_player->h;
        }
    }
}

void Game::resetGame() {

    list<Entity*> removeList;

    for(Entity* e : m_bullets) {
        removeList.push_back(e);
    }

    for(Entity* e : m_fighters) {
        removeList.push_back(e);
    }

    for(Entity* r : removeList) {
        delete r;
    }
    m_bullets.clear();
    m_fighters.clear();
    delete m_player;

    initializePlayer();
    initializeEnemy();
    m_gameResetTimer = FPS * 2;
}












