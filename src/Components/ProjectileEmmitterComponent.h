#pragma once

#include <glm/glm.hpp>
#include <SDL2/SDL.h>

struct ProjectileEmitterComponent
{

    glm::vec2 projectileVelocity;
    int frequency;
    int projectileDuration; // ms to keep alive
    bool isFriendly;
    int hitPercentageDamage;
    int lastEmmissionTime;

    ProjectileEmitterComponent(glm::vec2 projectileVelocity = glm::vec2(0), int frequency = 0, int projectileDuration = 10000, bool isFriendly = true, int hitPercentageDamage = 10)

    {
        this->projectileVelocity = projectileVelocity;
        this->frequency = frequency;
        this->projectileDuration = projectileDuration;
        this->isFriendly = isFriendly;
        this->hitPercentageDamage = hitPercentageDamage;
        this->lastEmmissionTime = SDL_GetTicks();
    }
};