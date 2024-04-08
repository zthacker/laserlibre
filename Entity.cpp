//
// Created by zthacker on 4/6/2024.
//

#include "Entity.h"

Entity::Entity(float x, float y, SDL_Texture* texture) {
    this->x = x;
    this->y = y;
    this->texture = texture;
}

Entity::~Entity() {
}
