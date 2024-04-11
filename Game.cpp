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
    initializeStarfield();
}

Game::~Game() {
    IMG_Quit();
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_DestroyTexture(m_playerBulletTexture);
    SDL_DestroyTexture(m_playerTexture);
    SDL_DestroyTexture(m_enemyTexture);
    SDL_DestroyTexture(m_background);
    SDL_DestroyTexture(m_explosionTexture);
    SDL_DestroyTexture(m_fontTexture);
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
    m_background = loadTexture(R"(C:\Users\zthacker\laserlibre\gfx\background.png)");
    m_explosionTexture = loadTexture(R"(C:\Users\zthacker\laserlibre\gfx\explosion.png)");
    m_fontTexture = loadTexture(R"(C:\Users\zthacker\laserlibre\gfx\font.png)");
}

void Game::initializePlayer() {
    this->m_player = new Entity(PLAYER_SPAWN_X, PLAYER_SPAWN_Y, m_playerTexture);
    SDL_QueryTexture(m_player->texture, nullptr, nullptr, &m_player->w, &m_player->h);

    this->m_player->reload = 0;
    this->m_player->health = PLAYER_HEALTH;
    this->m_player->side = SIDE_PLAYER;
    this->m_player->id = rand();
    this->m_player->name = "Player";
    this->m_fighters.push_back(m_player);
}

void Game::initializeEnemy() {
    this->enemySpawnTimer = 0;
}

void Game::initializeStarfield() {
    for (int i = 0; i < MAX_STARS; ++i) {
        m_stars[i].x = rand() % SCREEN_WIDTH;
        m_stars[i].y = rand() % SCREEN_HEIGHT;
        //random speed from 1 to 3;
        m_stars[i].speed = 1 + rand() % 8;
    }
}

void Game::prepareScene() {
    //background color
    SDL_SetRenderDrawColor(m_renderer, 96, 128, 255, 255);
    //clear out renderer
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

void Game::blitRect(SDL_Texture *texture, SDL_Rect* src, int x, int y) {
    SDL_Rect dest;

    dest.x = x;
    dest.y = y;
    dest.w = src->w;
    dest.h = src->h;

    SDL_RenderCopy(m_renderer, texture, src, &dest);
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

//Player needs to move (1)
void Game::keyPressDown(SDL_KeyboardEvent *event) {
    if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS)
    {
        m_keyboard[event->keysym.scancode] = 1;
    }
}

//Handling SDL_Events -- mainly for Keyboard right now
void Game::doInput() {
    //what is being checked here is that event type of the SDL. We want to know what keys were pressed in the event.
    // SDL has a long list of scancodes (ints for what key is pressed) and those need to be passed to our keyDown and keyUp
    //functions for verification and processing. The m_keyboard var holds all the scancodes (0 - MAX_KEYBOARD_KEYS) that
    // shouldn't be exceeded. Since those are ints are created in the array, this simple (and efficiently) sets
    // the scancode int to be 1 (pressed) and 0 (not pressed), which later we do movement on.
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
    doBackground();
    doStarfield();
    doPlayer();
    doEnemies();
    doFighters();
    doBullets();
    spawnEnemies();
    clipPlayer();
    doExplosions();
    doDebris();

    if(m_player == nullptr && --m_gameResetTimer <= 0) {
        resetGame();
    }

}

//Handles the Player's movement, firing and other vars
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
        if(f != m_player) {
            f->y = min(max(int(f->y), 0), SCREEN_HEIGHT - f->h);
            if(m_player != nullptr && --f->reload <= 0) {
                fireEnemeyBullet(f);
            }
        }
    }
}

void Game::doBullets() {

    list<Entity*> removeList;

    for(Entity* b : m_bullets) {
        //move bullet along it's delta X and Y
        b->x += b->dx;
        b->y += b->dy;

        //check if any of the bullets are leaving the screen or hit a fighter
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
        int spawnY = rand()%(SCREEN_HEIGHT-0 + 1) + 0;

        auto* enemy = new Entity(SCREEN_WIDTH,spawnY,m_enemyTexture);
        SDL_QueryTexture(enemy->texture, nullptr, nullptr, &enemy->w, &enemy->h);
        enemy->dx = -(2 + (rand() % 4));
        enemy->dy = -100 + (rand() % 200);
        enemy->dy /= 100;
        enemy->health = ENEMY_HEALTH;
        enemy->side = SIDE_ENEMY;
        enemy->id = rand();
        string text = "Enemy";
        enemy->name = text += to_string(enemy->id);

        //don't want the enemies to fire right away -- this is a 1 to 2 second window before they can fire
        enemy->reload = FPS * (1 +(rand() %3));

        //add to fighters list
        m_fighters.push_back(enemy);

        //spawn timer for Enemy -- some random number between 0-FPS(60)-1 + 30
        //this ends up with a spawn between 30 and 89ms -- .5sec - 1.5sec
        enemySpawnTimer = ENEMY_MIN_SPAWN_MS + (rand() % FPS);
    }
}

void Game::draw() {
    drawBackground();
    drawStarfield();
    drawBullets();
    drawFighters();
    drawDebris();
    drawExplosions();
    drawHud();
}

void Game::drawBackground() {
    SDL_Rect dest;
    for (int x = m_backgroundX; x < SCREEN_WIDTH; x += SCREEN_WIDTH) {
        dest.x = x;
        dest.y = 0;
        dest.w = SCREEN_WIDTH;
        dest.h = SCREEN_HEIGHT;

        SDL_RenderCopy(m_renderer, m_background, nullptr, &dest);
    }
}

void Game::drawStarfield() {
    int i, c;

    //go through each star
    for(i = 0; i < MAX_STARS; i++) {
        c = 32 * m_stars[i].speed;

        //draw a horizontal line of stars, setting the color to be the speed of the star (higher the speed, brighter the color)
        //using a line instead of a point here to make the stars a bit easier to see and provide a sense of speed
        SDL_SetRenderDrawColor(m_renderer, c, c, c, 255);
        SDL_RenderDrawLine(m_renderer, m_stars[i].x, m_stars[i].y, m_stars[i].x + 3, m_stars[i].y);
    }
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

void Game::drawDebris() {
    for(Entity* e : m_debris) {
        if(e != nullptr) {
            blitRect(e->texture, &e->rect, e->x, e->y);
        }
    }
}

void Game::drawExplosions() {
    //add blending -- meaning as we add layers, the colors will blend together and become brighter and reaching white
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_ADD);
    SDL_SetTextureBlendMode(m_explosionTexture, SDL_BLENDMODE_ADD);

    for(Entity* e : m_explosions) {
        SDL_SetTextureColorMod(m_explosionTexture, e->r, e->g, e->b);
        SDL_SetTextureAlphaMod(m_explosionTexture, e->a);

        blit(m_explosionTexture, e->x, e->y);
    }

    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE);
}

//x and y is where we want to draw the text at; r, g, b is the color; format is the text itself
void Game::drawText(int x, int y, int r, int g, int b, char *format, ...) {

    //to store chars and lengths later on
    int len, c;

    //used to specify a region of texture to use
    SDL_Rect rect;

    //holds varargs from input
    va_list args;

    //clear out text buffer
    memset(&m_drawTextBuffer, '\0', sizeof(m_drawTextBuffer));

    //format the string
    va_start(args, format);
    vsprintf(m_drawTextBuffer, format, args);
    va_end(args);

    //get the buffer text length to use for the rect
    len = strlen(m_drawTextBuffer);

    //setup the rect
    rect.w = GLYPH_WIDTH;
    rect.h = GLYPH_HEIGHT;
    rect.y = 0;

    //set the color
    SDL_SetTextureColorMod(m_fontTexture, r, g, b);

    //only doing ASCII characters, so if it's not supported, it's not drawn in blitRect
    for (int i = 0; i < len; ++i) {
        c = m_drawTextBuffer[i];
        if(c >= ' ' && c <= 'Z') {
            //x is the subtraction of the int value of (' ')
            //This is because a space is the first character in our font texture and therefore has an x position of 0.
            // This basically aligns our glyph coordinates on the x axis.
            rect.x = (c - ' ') * GLYPH_WIDTH;
            blitRect(m_fontTexture, &rect, x, y);
            x += GLYPH_WIDTH;
        }
    }
}

void Game::drawHud() {
    drawText(10,10,255,255,255,"SCORE: %03d", m_score);

    if(m_score > 0 && m_score == m_highscore) {
        drawText(960, 10, 0, 255, 0, "HIGH SCORE: %03d", m_highscore);
    } else {
        drawText(960, 10, 255, 255, 255, "HIGH SCORE: %03d", m_highscore);
    }
}

void Game::fireBullet() {
    auto* bullet = new Entity(m_player->x, m_player->y, m_playerBulletTexture);
    bullet->dx = PLAYER_BULLET_SPEED;
    bullet->health = 1;
    SDL_QueryTexture(bullet->texture, nullptr, nullptr, &bullet->w, &bullet->h);
    bullet->y += (m_player->h /2) - (bullet->h / 2);
    bullet->health = PLAYER_HEALTH;
    bullet->side = SIDE_PLAYER;
    bullet->id = rand();
    m_bullets.push_back(bullet);

    m_player->reload = PLAYER_RELOAD;
}

void Game::fireEnemeyBullet(Entity* e) {
    auto* bullet = new Entity(e->x, e->y, m_enemyBulletTexture);
    bullet->id = rand();
    bullet->health = 1;
    bullet->side = SIDE_ENEMY;
    SDL_QueryTexture(bullet->texture, nullptr, nullptr, &bullet->w, &bullet->h);

    //puts this in the middle of the entity's height and width
    bullet->x += (e->w / 2) - (bullet->w / 2);
    bullet->y += (e->h / 2) - (bullet->h / 2);

    //fire bullet from entity to player, getting the normalized steps of the slope to move
    calculateSlope(m_player->x + (m_player->w / 2), m_player->y + (m_player->h / 2), e->x, e->y, &bullet->dx, &bullet->dy);

    //create the delta of x and y for movement
    bullet->dx *= ENEMY_BULLET_SPEED;
    bullet->dy *= ENEMY_BULLET_SPEED;

    m_bullets.push_back(bullet);

    //fire anytime within the next 2 seconds (this'll be < 120, basically)
    e->reload = (rand() % FPS * 2);
}


int Game::collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    return (max(x1, x2) < min(x1 + w1, x2 + w2)) && (max(y1, y2) < min(y1 + h1, y2 + h2));
}

int Game::bulletHitFighter(Entity *b) {
    for(Entity* e : m_fighters) {
        if(e->side != b->side && collision(b->x, b->y, b->w, b->h, e->x, e->y, e->w, e->h)) {
            e->health--;
            b->health = 0;

            if(e->health <= 0) {
                b->health = 0;
                addExplosions(e->x, e->y, 32);
                addDebris(e);
                m_score++;
                m_highscore = max(m_score, m_highscore);
                return 1;
            }
            return 1;
        }
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

    for(Entity* e : m_explosions) {
        removeList.push_back(e);
    }

    for(Entity* e : m_debris) {
        removeList.push_back(e);
    }

    for(Entity* r : removeList) {
        delete r;
    }
    m_bullets.clear();
    m_fighters.clear();
    m_explosions.clear();
    m_debris.clear();

    initializePlayer();
    initializeEnemy();
    initializeStarfield();

    //3 second wait time before we restart the game
    m_gameResetTimer = FPS * 3;

    m_score = 0;
}

void Game::doBackground() {
    //decrements backgroundX and resets to 0 when it reaches negative screen width. This means when we draw it'll wrap around
    if(--m_backgroundX < -SCREEN_WIDTH) {
        m_backgroundX = 0;
    }
}

/*
 * Decrease the star's x value according to speed. When the value of x is less than 0 we'll return it to the other side of the screen.
 * We're taking into consideration what the negative value was at the time, rather than setting the star's x exactly to SCREEN_WIDTH.
 * This should help to avoid a situation where the stars could all start to line up over time.
 */
void Game::doStarfield() {
    for (int i = 0; i < MAX_STARS; ++i) {
        m_stars[i].x -= m_stars[i].speed;

        if(m_stars[i].x < 0) {
            m_stars[i].x = SCREEN_WIDTH + m_stars[i].x;
        }
    }
}

void Game::doExplosions() {
    list<Entity*> removeList;
    for(Entity* e : m_explosions) {
        if(e != nullptr) {
            //set the explosions x and y to the deltaX and deltaY to give it the spreading out effect and a little drift
            e->x += e->dx;
            e->y += e->dy;
            //once the alpha level (a) is 0 -- meaning it's faded out -- we'll delete it.
            if(--e->a <= 0) {
                removeList.push_back(e);
            }
        }
    }

    for(Entity* e : removeList) {
        m_explosions.remove(e);
        delete e;
    }

}

void Game::doDebris() {
    list<Entity*> removeList;
    for(Entity* e : m_debris) {
        e->x += e->dx;
        e->y += e->dy;
        //increase by .5 so they go down the screen
        e->dy += 0.5;
        //decrement life and see if it's <= 0
        if(--e->health <= 0) {
            removeList.push_back(e);
        }
    }
    for(Entity* e : removeList) {
        m_debris.remove(e);
        delete e;
    }
}

void Game::addExplosions(int x, int y, int num) {
    for (int i = 0; i < num; ++i) {
        // +/- 32 pixels in each direction
        int orgX = x + (rand() %32) - (rand() % 32);
        int orgY = y + (rand() %32) - (rand() % 32);

        //create explosion and add
        Entity* e = new Entity(orgX, orgY, nullptr);
        m_explosions.push_back(e);

        //get a +/- 9
        e->dx = (rand() % 10) - (rand() % 10);
        e->dy = (rand() % 10) - (rand() % 10);

        //divide by 10 to get us a +/- 0.9
        e->dx /= 10;
        e->dy /= 10;

        //do a random color -- SDL does color value 0 to 255
        switch (rand() % 4) {
            case 0:
                e->r = 255;
                break;
            case 1:
                e->r = 255;
                e->g = 128;
                break;
            case 2:
                e->r = 255;
                e->g = 255;
                break;
            default:
                e->r = 255;
                e->g = 255;
                e->b = 255;
                break;
        }

        //Alpha to random between 0 and 3 seconds
        e->a = rand() % FPS * 3;
    }
}

void Game::addDebris(Entity *e) {
    int x,y,w,h;

    //divide entitie's width and height by 2
    w = e->w / 2;
    h = e->h / 2;

    //loop through x and y, incrementing by values w and h
    for (y = 0; y <= h; y+=h) {
        for(x = 0; x <= w; x += w) {
            int orgX = e->x + e->w / 2;
            int orgY = e->y + e->h / 2;
            Entity* d = new Entity(orgX, orgY, e->texture);
            m_debris.push_back(d);

            //random velocity on x
            d->dx = (rand() % 5) - (rand() % 5);

            //always a negative velocity on y, because in doDebris, this eventually will go back down
            d->dy = -(5 + (rand() % 12));

            //2 second health (life)
            d->health = FPS * 2;

            /*
             * set the rect's x and y to those of our for loop, and the w and h to the w and h we worked out earlier.
             * In effect, we get the texture coordinates of our source texture in quarters.
             */
            d->rect.x = x;
            d->rect.y = y;
            d->rect.w = w;
            d->rect.h = h;
        }
    }
}















