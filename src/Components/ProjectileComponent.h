#pragma once

#include <SDL2/SDL.h>

struct ProjectileComponent
{
    bool isFriendly;
    int damage;
    int duration;
    int startTime;

    ProjectileComponent(bool isFriendly = false, int damage = 0, int duration = 0)
    {
        this->isFriendly = isFriendly;
        this->damage = damage;
        this->duration = duration;
        this->startTime = SDL_GetTicks();
    }
};
