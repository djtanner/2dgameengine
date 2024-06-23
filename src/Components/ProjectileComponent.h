#pragma once

#include <SDL2/SDL.h>

struct ProjectileComponent
{
    bool isFriendly;
    int damage;
    int duration;
    int startTime;
    int ownerEntityId;

    ProjectileComponent(bool isFriendly = false, int damage = 0, int duration = 0, int ownerEntityId = 0)

    {
        this->isFriendly = isFriendly;
        this->damage = damage;
        this->duration = duration;
        this->ownerEntityId = ownerEntityId;
        this->startTime = SDL_GetTicks();
    }
};
