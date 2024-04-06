//
// Created by zthacker on 4/6/2024.
//

#ifndef LASERLIBRE_ENTITY_H
#define LASERLIBRE_ENTITY_H

#include "common.h"

class Entity {
public:
    Entity(float x, float y, SDL_Texture* texture);
    ~Entity();

    float        x;
    float        y;
    int          w{};
    int          h{};
    float        dx{};
    float        dy{};
    int          health{};
    int          reload{};
    SDL_Texture* texture;
};


#endif //LASERLIBRE_ENTITY_H
