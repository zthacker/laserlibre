//
// Created by zthacker on 4/6/2024.
//

#ifndef LASERLIBRE_ENTITY_H
#define LASERLIBRE_ENTITY_H

#include "common.h"

using namespace std;

class Entity {
public:
    Entity(float x, float y, SDL_Texture* texture);
    ~Entity();

    int          id;
    string       name;
    float        x;
    float        y;
    int          w{};
    int          h{};
    float        dx{};
    float        dy{};
    int          health{};
    int          reload{};
    int          r,g,b,a;
    SDL_Texture* texture;
    SDL_Rect     rect;
    int          side;
};


#endif //LASERLIBRE_ENTITY_H
