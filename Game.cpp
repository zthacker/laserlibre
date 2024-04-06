//
// Created by zthacker on 4/5/2024.
//

#include "Game.h"

Game::Game() {
    m_state = LASER_STATE_SETUP;
    initializeSDL();
    initializePlayer();
}

Game::~Game() {
    IMG_Quit();
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
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

void Game::initializePlayer() {
    SDL_Texture* texture = loadTexture(R"(C:\Users\zthacker\laserlibre\gfx\player.png)");
    this->player = new Entity(100,100, texture);
    SDL_QueryTexture(player->texture, nullptr, nullptr, &player->w, &player->h);

    //fighterTail->next = player;
    //fighterTail = player;
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
        keyboard[event->keysym.scancode] = 0;
    }
}

void Game::keyPressDown(SDL_KeyboardEvent *event) {
    if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS)
    {
        keyboard[event->keysym.scancode] = 1;
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
    printf("doing logic\n");
    player->dx = player->dy = 0;

    if (player->reload > 0)
    {
        player->reload--;
    }

    if (keyboard[SDL_SCANCODE_UP])
    {
        player->dy = -PLAYER_SPEED;
    }

    if (keyboard[SDL_SCANCODE_DOWN])
    {
        player->dy = PLAYER_SPEED;
    }

    if (keyboard[SDL_SCANCODE_LEFT])
    {
        player->dx = -PLAYER_SPEED;
    }

    if (keyboard[SDL_SCANCODE_RIGHT])
    {
        player->dx = PLAYER_SPEED;
    }

//    if (keyboard[SDL_SCANCODE_LCTRL] && player->reload == 0)
//    {
//        fireBullet();
//    }

    player->x += player->dx;
    player->y += player->dy;
}

void Game::draw() {
    printf("doing draw\n");
    blit(player->texture, player->x, player->y);
}





