#pragma once

#include "../ECS/ECS.h"
#include "../Components/ProjectileComponent.h"
#include <SDL2/SDL.h>

class ProjectileLifecycleSystem : public System
{
public:
    ProjectileLifecycleSystem()
    {
        RequireComponent<ProjectileComponent>();
    }

    void Update()
    {
        auto entities = GetSystemEntities();
        for (auto &entity : entities)
        {
            auto &projectile = entity.GetComponent<ProjectileComponent>();
            if (SDL_GetTicks() - projectile.startTime >= projectile.duration)
            {
                entity.Kill();
            }
        }
    }
};