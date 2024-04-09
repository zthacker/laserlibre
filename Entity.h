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
    float        x;
    float        y;
    int          w{};
    int          h{};
    float        dx{};
    float        dy{};
    int          health{};
    int          reload{};
    SDL_Texture* texture;
    int          side;
};


#endif //LASERLIBRE_ENTITY_H
